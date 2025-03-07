#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cctype> 

// Enumeración de tipos de tokens
enum TokenType {
    // Palabras reservadas
    TOKEN_INCLUIR,// Incluir librerias
    TOKEN_PROGRAMA,// Gestión de bloques
    TOKEN_FIN_PROGRAMA,
    TOKEN_CONFIGURAR, 
    TOKEN_FIN_CONFIGURAR,
    TOKEN_BUCLE_PRINCIPAL, 
    TOKEN_FIN_BUCLE,
    TOKEN_SI,// Estructura de desición 
    TOKEN_ENTONCES, 
    TOKEN_SINO, 
    TOKEN_FIN_SI,
    TOKEN_PARA,// Bucle for  
    TOKEN_DESDE, 
    TOKEN_HASTA, 
    TOKEN_HACER, 
    TOKEN_FIN_PARA,
    TOKEN_MIENTRAS,// Bucle while
    TOKEN_FIN_MIENTRAS,
    TOKEN_REPETIR, // Bucle do while
    TOKEN_VECES,
    TOKEN_FUNCION,// Funciones
    TOKEN_FIN_FUNCION, 
    TOKEN_RETORNAR,
    // Tipos de datos
    TOKEN_ENTERO,
    TOKEN_FLOTANTE,
    TOKEN_CADENA,
    TOKEN_BOOLEANO, 
    TOKEN_VERDADERO,// valor booleano 1
    TOKEN_FALSO,// valor booleano 0
    // Literales e identificadores
    TOKEN_IDENTIFICADOR,
    TOKEN_NUMERO_LIT,// tipo entero
    TOKEN_DECIMAL_LIT,// tipo flotante
    TOKEN_CADENA_LIT,
    TOKEN_CONFIGURAR_PIN,// Para controlar la tarjeta Arduino
    TOKEN_ESCRIBIR,
    TOKEN_ESPERAR,
    // Símbolos y operadores
    TOKEN_PARENTESIS_IZQ,
    TOKEN_PARENTESIS_DER,
    TOKEN_LLAVE_IZQ,
    TOKEN_LLAVE_DER,
    TOKEN_COMA,
    TOKEN_ASIGNACION,
    TOKEN_IGUALDAD,
    TOKEN_DESIGUALDAD,
    TOKEN_MAYOR_QUE,
    TOKEN_MAYOR_IGUAL,
    TOKEN_MENOR_QUE,
    TOKEN_MENOR_IGUAL,
    TOKEN_MAS,
    TOKEN_MENOS,
    TOKEN_DIV,
    TOKEN_MULT,
    TOKEN_PUNTO_COMA,
    TOKEN_EOF, // END OF FILE (fin de archivo)
};

// Estructura para representar un token
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

// Función para identificar palabras reservadas
TokenType identificarPalabraReservada(const std::string& valor) {
    if (valor == "incluir") return TOKEN_INCLUIR;
    if (valor == "programa") return TOKEN_PROGRAMA;
    if (valor == "fin_programa") return TOKEN_FIN_PROGRAMA;
    if (valor == "configurar") return TOKEN_CONFIGURAR;
    if (valor == "fin_configurar") return TOKEN_FIN_CONFIGURAR;
    if (valor == "bucle_principal") return TOKEN_BUCLE_PRINCIPAL;
    if (valor == "fin_bucle") return TOKEN_FIN_BUCLE;
    if (valor == "si") return TOKEN_SI;
    if (valor == "entonces") return TOKEN_ENTONCES;
    if (valor == "sino") return TOKEN_SINO;
    if (valor == "fin_si") return TOKEN_FIN_SI;
    if (valor == "para") return TOKEN_PARA;
    if (valor == "desde") return TOKEN_DESDE;
    if (valor == "hasta") return TOKEN_HASTA;
    if (valor == "hacer") return TOKEN_HACER;
    if (valor == "fin_para") return TOKEN_FIN_PARA;
    if (valor == "mientras") return TOKEN_MIENTRAS;
    if (valor == "fin_mientras") return TOKEN_FIN_MIENTRAS;
    if (valor == "repetir") return TOKEN_REPETIR;
    if (valor == "veces") return TOKEN_VECES;
    if (valor == "funcion") return TOKEN_FUNCION;
    if (valor == "fin_funcion") return TOKEN_FIN_FUNCION;
    if (valor == "retornar") return TOKEN_RETORNAR;
    if (valor == "entero") return TOKEN_ENTERO;
    if (valor == "decimal") return TOKEN_FLOTANTE;
    if (valor == "booleano") return TOKEN_BOOLEANO;
    if (valor == "verdadero") return TOKEN_VERDADERO;
    if (valor == "falso") return TOKEN_FALSO;
    if (valor == "configurar_pin") return TOKEN_CONFIGURAR_PIN;
    if (valor == "escribir") return TOKEN_ESCRIBIR;
    if (valor == "esperar") return TOKEN_ESPERAR;
    return TOKEN_IDENTIFICADOR; // Si no es una palabra reservada, es un identificador
}

// Función para realizar el análisis léxico
std::vector<Token> analizadorLexico(std::ifstream& archivo) {
    std::vector<Token> tokens;
    char c;
    int linea = 1;
    int columna = 1;
    std::string buffer;

    while (archivo.get(c)) {
        // Ignorar espacios en blanco
        if (std::isspace(c)) {
            if (c == '\n') {
                linea++;
                columna = 1;
            } else {
                columna++;
            }
            continue;
        }

        // Detectar comentarios
        if (c == '/') {
            char nextChar = archivo.peek();
            if (nextChar == '/') { // Comentario de una línea
                archivo.get(); // Consume el segundo '/'
                while (archivo.get(c) && c != '\n') {
                    // Ignorar todos los caracteres hasta el final de la línea
                }
                if (c == '\n') {
                    linea++;
                    columna = 1;
                }
                continue;
            } else if (nextChar == '*') { // Comentario de múltiples líneas
                archivo.get(); // Consume el '*'
                bool comentarioTerminado = false;
                while (archivo.get(c)) {
                    if (c == '\n') {
                        linea++;
                        columna = 1;
                    } else {
                        columna++;
                    }
                    if (c == '*' && archivo.peek() == '/') {
                        archivo.get(); // Consume el '/'
                        comentarioTerminado = true;
                        break;
                    }
                }
                if (!comentarioTerminado) {
                    std::cerr << "Error lexico: comentario sin terminar en linea " << linea << ", columna " << columna << std::endl;
                }
                continue;
            }
        }

        // Detectar cadenas de texto
        if (c == '"') {
            buffer.clear();
            columna++;

            while (archivo.get(c) && c != '"') {
                if (c == '\n') {
                    std::cerr << "Error lexico: cadena sin terminar en linea " << linea << ", columna " << columna << std::endl;
                    break;
                }
                buffer += c;
                columna++;
            }
            if (c != '"') {
                std::cerr << "Error lexico: cadena sin terminar en linea " << linea << ", columna " << columna << std::endl;
            } else {
                tokens.push_back(Token{TOKEN_CADENA_LIT, buffer, linea, columna - static_cast<int>(buffer.length())});
                columna++;
            }
            continue;
        }

        // Detectar números enteros o decimales
        if (std::isdigit(c)) {
            buffer.clear();
            buffer += c;
            columna++;

            while (archivo.get(c) && (std::isdigit(c) || c == '.')) {
                if (c == '.' && buffer.find('.') != std::string::npos) {
                    std::cerr << "Error lexico: numero con multiples puntos en linea " << linea << ", columna " << columna << std::endl;
                    break;
                }
                buffer += c;
                columna++;
            }
            archivo.unget(); // Devolver el último carácter leído

            if (buffer.find('.') != std::string::npos) {
                tokens.push_back(Token{TOKEN_DECIMAL_LIT, buffer, linea, columna - static_cast<int>(buffer.length())});
            } else {
                tokens.push_back(Token{TOKEN_NUMERO_LIT, buffer, linea, columna - static_cast<int>(buffer.length())});
            }
            continue;
        }

        // Detectar identificadores o palabras reservadas
        if (std::isalpha(c) || c == '_') {
            buffer.clear();
            buffer += c;
            columna++;

            while (archivo.get(c) && (std::isalnum(c) || c == '_')) {
                buffer += c;
                columna++;
            }
            archivo.unget(); // Devolver el último carácter leído

            TokenType tipo = identificarPalabraReservada(buffer);
            tokens.push_back(Token{tipo, buffer, linea, columna - static_cast<int>(buffer.length())});
            continue;
        }

        // Detectar símbolos especiales y operadores compuestos
        TokenType tipo;
        switch (c) {
            case '(': tipo = TOKEN_PARENTESIS_IZQ; break;
            case ')': tipo = TOKEN_PARENTESIS_DER; break;
            case '{': tipo = TOKEN_LLAVE_IZQ; break;
            case '}': tipo = TOKEN_LLAVE_DER; break;
            case ';': tipo = TOKEN_PUNTO_COMA; break;
            case ',': tipo = TOKEN_COMA; break;
            case '=':
                if (archivo.peek() == '=') { // Operador de igualdad (==)
                    archivo.get(); // Consume el segundo '='
                    tipo = TOKEN_IGUALDAD;
                    tokens.push_back(Token{tipo, "==", linea, columna});
                    columna += 2;
                    continue;
                } else { // Operador de asignación (=)
                    tipo = TOKEN_ASIGNACION;
                }
                break;
            case '!':
                if (archivo.peek() == '=') { // Operador de desigualdad (!=)
                    archivo.get(); // Consume el '='
                    tipo = TOKEN_DESIGUALDAD;
                    tokens.push_back(Token{tipo, "!=", linea, columna});
                    columna += 2;
                    continue;
                } else {
                    std::cerr << "Error lexico: caracter inesperado '!' en linea " << linea << ", columna " << columna << std::endl;
                    columna++;
                    continue;
                }
                break;
            case '>':
                if (archivo.peek() == '=') { // Operador de mayor igual (>=)
                    archivo.get();
                    tokens.push_back(Token{TOKEN_MAYOR_IGUAL, ">=", linea, columna});
                    columna += 2;
                } else { // Operador de mayor que (>)
                    tokens.push_back(Token{TOKEN_MAYOR_QUE, ">", linea, columna});
                    columna++;
                }
                continue;
            case '<':
                if (archivo.peek() == '=') { // Operador de menor igual (<=)
                    archivo.get();
                    tokens.push_back(Token{TOKEN_MENOR_IGUAL, "<=", linea, columna});
                    columna += 2;
                } else { //  Operador de menor que (<)
                    tokens.push_back(Token{TOKEN_MENOR_QUE, "<", linea, columna});
                    columna++;
                }
                continue;
            case '+': // Operador de suma
                tokens.push_back(Token{TOKEN_MAS, "+", linea, columna});
                columna++;
                continue;
            case '-': // Operador de resta
                tokens.push_back(Token{TOKEN_MENOS, "-", linea, columna});
                columna++;
                continue;
            case '*': // Operador de multiplicación
                tokens.push_back(Token{TOKEN_MULT, "*", linea, columna});
                columna++;
                continue;
            case '/':
                // Validar si no es comentario
                if (archivo.peek() != '/' && archivo.peek() != '*') {
                    tokens.push_back(Token{TOKEN_DIV, "/", linea, columna});
                    columna++;
                    continue;
                }
            default:
                std::cerr << "Error lexico: caracter inesperado '" << c << "' en linea " << linea << ", columna " << columna << std::endl;
                columna++;
                continue;
        }
        tokens.push_back(Token{tipo, std::string(1, c), linea, columna});
        columna++;
    }

    // Añadir token de fin de archivo
    tokens.push_back(Token{TOKEN_EOF, "", linea, columna});

    return tokens;
}

int main() {
    std::string ruta;
    std::cout << "Ingrese la ruta del archivo: ";
    std::getline(std::cin, ruta);

    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return 1;
    }

    // Realizar el análisis léxico
    std::vector<Token> tokens = analizadorLexico(archivo);

    // Cerrar el archivo
    archivo.close();

    // Imprimir los tokens reconocidos
    for (const auto& token : tokens) {
        std::cout << "Token: " << token.value << " (Tipo: " << token.type << ", Linea: " << token.line << ", Columna: " << token.column << ")" << std::endl;
    }

    return 0;
}