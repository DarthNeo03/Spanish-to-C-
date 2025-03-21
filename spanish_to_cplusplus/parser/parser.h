#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "simbolos.h"
#include "errores.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <iomanip>

//--------------------------------------------------
// Estructuras del AST (Árbol de Sintaxis Abstracta)
//--------------------------------------------------
enum TipoNodo {
    NODO_PROGRAMA,
    NODO_DECLARACION,
    NODO_ASIGNACION,
    NODO_EXPRESION,
    NODO_LITERAL,
    NODO_VARIABLE
};

struct Nodo {
    virtual ~Nodo() = default;
    TipoNodo tipo;
    int linea;
    int columna;
};

struct NodoExpresion : public Nodo {
    TipoDato tipoDato = INDEFINIDO;
};

struct NodoLiteral : public NodoExpresion {
    std::string valor;
};

struct NodoVariable : public NodoExpresion {
    std::string nombre;
};

struct NodoDeclaracion : public Nodo {
    std::string identificador;
    std::unique_ptr<NodoExpresion> expresion;
    TipoDato tipoDeclarado;
};

struct NodoPrograma : public Nodo {
    std::vector<std::unique_ptr<Nodo>> declaraciones;
};

//--------------------------------------------------
// Clase principal del Parser
//--------------------------------------------------
class Parser {
private:
    std::vector<Token> tokens;
    size_t posActual = 0;
    TablaSimbolos tablaSimbolos;
    std::unique_ptr<NodoPrograma> ast;

    //Para insertar los errores en la tabla
    std::vector<Error>& errores;

    // Nueva estrategia de recuperación
    void recuperarError() {
        size_t posInicial = posActual;
        // Avanza hasta encontrar un punto de sincronización
        while (posActual < tokens.size() && 
            actual().type != TOKEN_PUNTO_COMA && 
            actual().type != TOKEN_LLAVE_DER && 
            actual().type != TOKEN_FIN_PROGRAMA) {
            posActual++;
        }
        if (posInicial == posActual) posActual++; // Prevenir loop
    }

    // Helpers de análisis
    Token& actual() {
        if (posActual >= tokens.size()) {
            static Token tokenError = {TOKEN_DESCONOCIDO, "", 0, 0};
            if (posActual == tokens.size()) { // Solo registrar error una vez
                errores.push_back({"Fin de archivo inesperado", tokens.back().line, tokens.back().column, "Sintáctico"});
            }
            posActual++; // Evitar múltiples llamadas
            return tokenError;
        }
        return tokens[posActual];
    }

    void avanzar() { if (posActual < tokens.size()) posActual++; }

    void coincidir(TokenType esperado) {
        if (actual().type != esperado) {
            errores.push_back({
                "Token inesperado. Esperado: " + tokenTypeToString(esperado) + 
                ", Encontrado: " + tokenTypeToString(actual().type),
                actual().line, actual().column, "Sintáctico"
            });
            recuperarError();
            return;
        }
        avanzar();
    }

public:
    Parser(std::vector<Token> tokens, std::vector<Error>& errores) 
        : tokens(std::move(tokens)), errores(errores), ast(new NodoPrograma) {
        // Insertar token de fin automáticamente
        if (!this->tokens.empty() && this->tokens.back().type != TOKEN_FIN_PROGRAMA) {
            this->tokens.push_back({TOKEN_FIN_PROGRAMA, "", 0, 0});
        }
    }

    std::unique_ptr<NodoPrograma> analizar() {
        programa();
        return std::move(ast);
    }

    // Método para imprimir tabla de símbolos
    inline void imprimirTablaSimbolos() const {
        std::cout << "\nTabla de Simbolos:\n";
        std::cout << std::left 
            << std::setw(20) << "Nombre"
            << std::setw(15) << "Tipo"
            << std::setw(10) << "Linea"
            << "Valor\n";
            
        for (const auto& [nombre, simbolo] : tablaSimbolos.simbolos) {
            std::cout << std::setw(20) << nombre
                    << std::setw(15) << tipoDatoToString(simbolo.tipo)
                    << std::setw(10) << simbolo.lineaDeclaracion
                    << valorToString(simbolo.valor) << "\n";
        }
    }

private:
    void programa() {
        coincidir(TOKEN_PROGRAMA);
        coincidir(TOKEN_IDENTIFICADOR);
        
        while (actual().type != TOKEN_FIN_PROGRAMA) {
            if (esTipoDato(actual().type)) {
                ast->declaraciones.push_back(declaracionVariable());
            } else {
                errores.push_back({
                    "Declaración inválida",
                    actual().line, actual().column, "Sintáctico"
                });
                recuperarError();
            }
        }
        
        coincidir(TOKEN_FIN_PROGRAMA);
    }

    std::unique_ptr<Nodo> declaracionVariable() {
        auto nodo = std::make_unique<NodoDeclaracion>();
        nodo->tipo = NODO_DECLARACION;
        nodo->linea = actual().line;
        nodo->columna = actual().column;

        // Tipo de dato
        nodo->tipoDeclarado = tokenToTipoDato(actual().type);
        avanzar();

        // Identificador
        if (actual().type != TOKEN_IDENTIFICADOR) {
            errores.push_back({
                "Se esperaba identificador después de tipo de dato",
                actual().line, actual().column, "Sintáctico"
            });
            recuperarError();
            return nullptr;
        }
        nodo->identificador = actual().value;
        verificarDeclaracionDuplicada(nodo->identificador);
        avanzar();

        // Asignación opcional
        if (actual().type == TOKEN_ASIGNACION) {
            avanzar();
            nodo->expresion = expresion();
        }

        coincidir(TOKEN_PUNTO_COMA);

        // Registrar en tabla de símbolos
        registrarEnTablaSimbolos(*nodo);

        return nodo;
    }

    std::unique_ptr<NodoExpresion> expresion() {
        auto expr = std::make_unique<NodoExpresion>();
        expr->linea = actual().line;
        expr->columna = actual().column;

        if (actual().type == TOKEN_IDENTIFICADOR) {
            auto var = std::make_unique<NodoVariable>();
            var->nombre = actual().value;
            verificarVariableDeclarada(var->nombre);
            var->tipoDato = obtenerTipoVariable(var->nombre);
            expr = std::move(var);
            avanzar();
        } else if (esLiteral(actual().type)) {
            auto lit = std::make_unique<NodoLiteral>();
            lit->valor = actual().value;
            lit->tipoDato = tokenToTipoDato(actual().type);
            expr = std::move(lit);
            avanzar();
        } else {
            errores.push_back({
                "Expresión inválida",
                actual().line, actual().column, "Sintáctico"
            });
            recuperarError();
            return nullptr;
        }

        return expr;
    }

    //--------------------------------------------------
    // Funciones de soporte semántico
    //--------------------------------------------------
    void verificarDeclaracionDuplicada(const std::string& nombre) {
        if (tablaSimbolos.buscar(nombre)) {
            errores.push_back({
                "Variable ya declarada: " + nombre,
                actual().line, actual().column, "Semántico"
            });
        }
    }

    void verificarVariableDeclarada(const std::string& nombre) {
        if (!tablaSimbolos.buscar(nombre)) {
            errores.push_back({
                "Variable no declarada: " + nombre,
                actual().line, actual().column, "Semántico"
            });
        }
    }

    TipoDato obtenerTipoVariable(const std::string& nombre) {
        auto simbolo = tablaSimbolos.buscar(nombre);
        return simbolo ? simbolo->tipo : INDEFINIDO;
    }

    void registrarEnTablaSimbolos(const NodoDeclaracion& declaracion) {
        Simbolo simbolo{
            declaracion.identificador,
            declaracion.tipoDeclarado,
            declaracion.linea,
            {} // Valor inicial
        };
        tablaSimbolos.insertar(simbolo);
    }

    //--------------------------------------------------
    // Funciones utilitarias
    //--------------------------------------------------
    static bool esTipoDato(TokenType type) {
        return type == TOKEN_ENTERO || type == TOKEN_FLOTANTE || 
               type == TOKEN_CADENA || type == TOKEN_BOOLEANO;
    }

    static bool esLiteral(TokenType type) {
        return type == TOKEN_NUMERO_LIT || type == TOKEN_DECIMAL_LIT || 
               type == TOKEN_CADENA_LIT || type == TOKEN_VERDADERO || 
               type == TOKEN_FALSO;
    }

    static TipoDato tokenToTipoDato(TokenType type) {
        switch(type) {
            case TOKEN_ENTERO:     return ENTERO;
            case TOKEN_FLOTANTE:   return DECIMAL;
            case TOKEN_CADENA:     return CADENA;
            case TOKEN_BOOLEANO:   return BOOLEANO;
            case TOKEN_NUMERO_LIT: return ENTERO;
            case TOKEN_DECIMAL_LIT:return DECIMAL;
            case TOKEN_CADENA_LIT: return CADENA;
            case TOKEN_VERDADERO:  return BOOLEANO;
            case TOKEN_FALSO:      return BOOLEANO;
            default:               return INDEFINIDO;
        }
    }
};

#endif // PARSER_H
