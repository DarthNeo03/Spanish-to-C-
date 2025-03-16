#include "lexer.h"
#include "simbolos.h"
#include <stdexcept>

class ErrorSintactico : public std::runtime_error {
public:
    int linea;
    int columna;

    ErrorSintactico(const std::string& msg, int ln, int col)
        : std::runtime_error(msg + " (Línea: " + std::to_string(ln) + ", Columna: " + std::to_string(col) + ")"), 
          linea(ln), columna(col) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t posActual = 0;
    TablaSimbolos tablaSimbolos;

    Token& actual() {
        if (posActual >= tokens.size()) 
            throw ErrorSintactico("Fin de archivo inesperado", tokens.back().line, tokens.back().column);
        return tokens[posActual];
    }

    void avanzar() {
        if (posActual < tokens.size()) posActual++;
    }

    void coincidir(TokenType esperado) {
        if (actual().type != esperado) {
            throw ErrorSintactico(
                "Token inesperado. Se esperaba: " + tokenTypeToString(esperado) + 
                ", se encontró: " + tokenTypeToString(actual().type),
                actual().line, actual().column
            );
        }
        avanzar();
    }

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    void analizar() {
        programa();
    }

private:
    void programa() {
        coincidir(TOKEN_PROGRAMA);
        coincidir(TOKEN_IDENTIFICADOR);
        
        // Procesar inclusiones
        while (actual().type == TOKEN_INCLUIR) {
            inclusion();
        }

        // Procesar bloques
        if (actual().type == TOKEN_CONFIGURAR) {
            bloqueConfiguracion();
        }

        bloquePrincipal();
        coincidir(TOKEN_FIN_PROGRAMA);
    }

    void inclusion() {
        coincidir(TOKEN_INCLUIR);
        coincidir(TOKEN_MENOR_QUE);
        coincidir(TOKEN_IDENTIFICADOR);
        coincidir(TOKEN_MAYOR_QUE);
    }

    void bloqueConfiguracion() {
        coincidir(TOKEN_CONFIGURAR);
        
        while (actual().type != TOKEN_FIN_CONFIGURAR) {
            if (actual().type == TOKEN_CONFIGURAR_PIN) {
                configuracionPin();
            }
            else {
                throw ErrorSintactico("Declaración inválida en bloque de configuración", 
                    actual().line, actual().column);
            }
        }
        
        coincidir(TOKEN_FIN_CONFIGURAR);
    }

    void configuracionPin() {
        coincidir(TOKEN_CONFIGURAR_PIN);
        coincidir(TOKEN_PARENTESIS_IZQ);
        
        // Número de pin
        if (actual().type != TOKEN_NUMERO_LIT) {
            throw ErrorSintactico("Se esperaba número de pin", actual().line, actual().column);
        }
        avanzar();
        coincidir(TOKEN_COMA);
        
        // Modo del pin
        if (actual().type != TOKEN_CADENA_LIT) {
            throw ErrorSintactico("Se esperaba modo de pin (cadena)", actual().line, actual().column);
        }
        avanzar();
        
        coincidir(TOKEN_PARENTESIS_DER);
        coincidir(TOKEN_PUNTO_COMA);
    }

    void bloquePrincipal() {
        coincidir(TOKEN_BUCLE_PRINCIPAL);
        
        while (actual().type != TOKEN_FIN_BUCLE) {
            declaracion();
        }
        
        coincidir(TOKEN_FIN_BUCLE);
    }

    void declaracion() {
        switch (actual().type) {
            case TOKEN_ENTERO:
            case TOKEN_FLOTANTE:
            case TOKEN_CADENA:
            case TOKEN_BOOLEANO:
                declaracionVariable();
                break;
            case TOKEN_ESCRIBIR:
                sentenciaEscritura();
                break;
            case TOKEN_ESPERAR:
                sentenciaEspera();
                break;
            case TOKEN_SI:
                sentenciaSi();
                break;
            default:
                throw ErrorSintactico("Declaración no válida", actual().line, actual().column);
        }
    }

    void declaracionVariable() {
        TipoDato tipo;
        switch (actual().type) {
            case TOKEN_ENTERO: tipo = ENTERO; break;
            case TOKEN_FLOTANTE: tipo = DECIMAL; break;
            case TOKEN_CADENA: tipo = CADENA; break;
            case TOKEN_BOOLEANO: tipo = BOOLEANO; break;
            default: 
                throw ErrorSintactico("Tipo de dato no válido", actual().line, actual().column);
        }
        avanzar();
        
        std::string nombre = actual().value;
        coincidir(TOKEN_IDENTIFICADOR);
        
        // Insertar en tabla de símbolos
        Simbolo simbolo{nombre, tipo, {}, actual().line};
        tablaSimbolos.insertar(simbolo);
        
        if (actual().type == TOKEN_ASIGNACION) {
            avanzar();
            expresion();
        }
        
        coincidir(TOKEN_PUNTO_COMA);
    }

    void sentenciaEscritura() {
        coincidir(TOKEN_ESCRIBIR);
        coincidir(TOKEN_PARENTESIS_IZQ);
        
        while (actual().type != TOKEN_PARENTESIS_DER) {
            expresion();
            if (actual().type == TOKEN_COMA) {
                avanzar();
            }
        }
        
        coincidir(TOKEN_PARENTESIS_DER);
        coincidir(TOKEN_PUNTO_COMA);
    }

    
};
