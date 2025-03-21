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
    NODO_VARIABLE,
    NODO_INCLUIR,
    NODO_CONFIGURAR,
    NODO_BUCLE_PRINCIPAL,
    NODO_LLAMADA_FUNCION
};

// Tipo básico para expresiones
enum class TipoExpresion {
    VARIABLE,
    LITERAL,
    OPERACION
};

struct Nodo {
    virtual ~Nodo() = default;
    TipoNodo tipo;
    int linea;
    int columna;
};

struct NodoExpresion : public Nodo {
    TipoExpresion tipo;
};

struct NodoLiteral : public NodoExpresion {
    std::string valor;
    std::string tipoDato;  // Nuevo miembro requerido
    
    NodoLiteral() {
        tipo = TipoExpresion::LITERAL;
    }
};

struct NodoVariable : public NodoExpresion {
    std::string nombre;
    std::string tipoDato;  // Nuevo miembro requerido
    
    NodoVariable() {
        tipo = TipoExpresion::VARIABLE;
    }
};

struct NodoLlamadaFuncion : public Nodo {
    std::string nombre;
    std::vector<std::unique_ptr<NodoExpresion>> argumentos;
};

struct NodoDeclaracion : public Nodo {
    std::string identificador;
    std::unique_ptr<NodoExpresion> expresion;
    TipoDato tipoDeclarado;
};

struct NodoPrograma : public Nodo {
    std::vector<std::unique_ptr<Nodo>> declaraciones;
};

struct NodoIncluir : public Nodo {
    std::string archivo;
};

struct NodoConfigurar : public Nodo {
    std::vector<std::unique_ptr<Nodo>> instrucciones;
};

struct NodoBuclePrincipal : public Nodo {
    std::vector<std::unique_ptr<Nodo>> instrucciones;
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

    bool finalBloque() {
        return actual().type == TOKEN_FIN_CONFIGURAR || 
               actual().type == TOKEN_FIN_BUCLE ||
               actual().type == TOKEN_FIN_PROGRAMA;
    }

    bool funcionesBuclePrincipal() {
        return actual().type == TOKEN_ESCRIBIR || 
               actual().type == TOKEN_ESPERAR;
    }

    // Helpers de análisis
    Token& actual() {
        if (posActual >= tokens.size()) {
            static Token tokenError = {TOKEN_DESCONOCIDO, "", 0, 0};
            if (posActual == tokens.size()) { // Solo registrar error una vez
                errores.push_back({"Fin de archivo inesperado", tokens.back().line, tokens.back().column, "Sintactico"});
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
                actual().line, actual().column, "Sintactico"
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
            switch (actual().type) {
                case TOKEN_INCLUIR:
                    ast->declaraciones.push_back(funcionIncluir());
                    break;
                case TOKEN_ENTERO:
                case TOKEN_FLOTANTE:
                case TOKEN_CADENA:
                case TOKEN_BOOLEANO:
                    ast->declaraciones.push_back(declaracionVariable());
                    break;
                case TOKEN_CONFIGURAR:
                    ast->declaraciones.push_back(funcionConfigurar());
                    break;
                case TOKEN_BUCLE_PRINCIPAL:
                    ast->declaraciones.push_back(funcionBuclePrincipal());
                    break;
                default:
                    errores.push_back({
                        "Declaracion o instruccion invalida",
                        actual().line, actual().column, "Sintactico"
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
                "Se esperaba identificador despues de tipo de dato",
                actual().line, actual().column, "Sintactico"
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
                "Expresion invalida",
                actual().line, actual().column, "Sintactico"
            });
            recuperarError();
            return nullptr;
        }

        return expr;
    }

    std::unique_ptr<Nodo> funcionIncluir() {
        auto nodo = std::make_unique<NodoIncluir>();
        nodo->tipo = NODO_INCLUIR;
        nodo->linea = actual().line;
        nodo->columna = actual().column;
        
        avanzar(); // Consumir TOKEN_INCLUIR
        coincidir(TOKEN_MENOR_QUE);
        
        if (actual().type != TOKEN_CADENA_LIT) {
            errores.push_back({
                "Se esperaba nombre de archivo entre comillas",
                actual().line, actual().column, "Sintactico"
            });
            recuperarError();
            return nullptr;
        }
        
        nodo->archivo = actual().value;
        avanzar(); // Consumir cadena
        coincidir(TOKEN_MAYOR_QUE);
        
        return nodo;
    }
    
    std::unique_ptr<Nodo> funcionConfigurar() {
        auto nodo = std::make_unique<NodoConfigurar>();
        nodo->tipo = NODO_CONFIGURAR;
        nodo->linea = actual().line;
        nodo->columna = actual().column;
        
        avanzar(); // Consumir TOKEN_CONFIGURAR
        
        while (actual().type != TOKEN_FIN_CONFIGURAR && !finalBloque()) {
            if (actual().type == TOKEN_CONFIGURAR_PIN) {
                auto llamada = llamadaFuncion();
                if (llamada) {
                    nodo->instrucciones.push_back(std::move(llamada));
                }
            } else {
                errores.push_back({
                    "Instruccion invalida en bloque configurar",
                    actual().line, actual().column, "Sintactico"
                });
                recuperarError();
            }
        }
        
        coincidir(TOKEN_FIN_CONFIGURAR);
        return nodo;
    }
    
    std::unique_ptr<Nodo> funcionBuclePrincipal() {
        auto nodo = std::make_unique<NodoBuclePrincipal>();
        nodo->tipo = NODO_BUCLE_PRINCIPAL;
        nodo->linea = actual().line;
        nodo->columna = actual().column;
        
        avanzar(); // Consumir TOKEN_BUCLE_PRINCIPAL
        
        while (actual().type != TOKEN_FIN_CONFIGURAR && !finalBloque()) {
            /*std::cout << "Token S: " << actual().value 
            << " (Tipo: " << actual().type
            << ", Linea: " << actual().line 
            << ", Columna: " << actual().column << ")" << std::endl;*/
            if (funcionesBuclePrincipal()) {
                auto llamada = llamadaFuncion();
                if (llamada) {
                    nodo->instrucciones.push_back(std::move(llamada));
                }
            } else {
                errores.push_back({
                    "Instruccion invalida en bucle principal",
                    actual().line, actual().column, "Sintactico"
                });
                recuperarError();
            }
        }
        
        coincidir(TOKEN_FIN_BUCLE);
        return nodo;
    }

    //--------------------------------------------------
    // Función mejorada llamadaFuncion() para soportar parámetros
    //--------------------------------------------------
    std::unique_ptr<NodoLlamadaFuncion> llamadaFuncion() {
        auto nodo = std::make_unique<NodoLlamadaFuncion>();
        nodo->tipo = NODO_LLAMADA_FUNCION;
        nodo->linea = actual().line;
        nodo->columna = actual().column;
        nodo->nombre = actual().value;
        avanzar(); // Consumir el identificador
        
        // Manejo de paréntesis de apertura
        if (actual().type != TOKEN_PARENTESIS_IZQ) {
            errores.push_back({
                "Se esperaba '(' despues de nombre de funcion",
                actual().line, actual().column, "Sintactico"
            });
            recuperarError();
            return nullptr;
        }
        avanzar();// Consumir paréntesis izquierdo
        
        // Procesamiento de argumentos
        while (actual().type != TOKEN_PARENTESIS_DER && !finalBloque()) {
            auto expr = expresion();
            if (expr) {
                nodo->argumentos.push_back(std::move(expr));
            }
            
            if (actual().type == TOKEN_COMA) {
                avanzar();
            } else if (actual().type != TOKEN_PARENTESIS_DER) {
                errores.push_back({
                    "Se esperaba ',' o ')' en lista de argumentos",
                    actual().line, actual().column, "Sintactico"
                });
                recuperarError();
            }
        }
        
        // Manejo de paréntesis de cierre
        if (actual().type != TOKEN_PARENTESIS_DER) {
            errores.push_back({
                "Llave no cerrada en llamada a funcion",
                actual().line, actual().column, "Sintactico"
            });
        } else {
            avanzar();
        }

        if (actual().type != TOKEN_PUNTO_COMA) {
            errores.push_back({"Se esperaba ';' despues de llamada a funcion", 
                              actual().line, actual().column, "Sintactico"});
        } else {
            avanzar();  // Consumir el ';'
        }
        
        return nodo;
    }

    //--------------------------------------------------
    // Funciones de soporte semántico
    //--------------------------------------------------
    void verificarDeclaracionDuplicada(const std::string& nombre) {
        if (tablaSimbolos.buscar(nombre)) {
            errores.push_back({
                "Variable ya declarada: " + nombre,
                actual().line, actual().column, "Semantico"
            });
        }
    }

    void verificarVariableDeclarada(const std::string& nombre) {
        if (!tablaSimbolos.buscar(nombre)) {
            errores.push_back({
                "Variable no declarada: " + nombre,
                actual().line, actual().column, "Semantico"
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
