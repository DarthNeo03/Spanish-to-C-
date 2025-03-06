#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cctype> 

// Enumeración de tipos de tokens
enum TokenType {
    TOKEN_ENTERO,       
    TOKEN_ESCRIBIR,     
    TOKEN_ESPERAR,
    TOKEN_REPETIR,
    TOKEN_VECES,
    TOKEN_CONFIGURAR_PIN,
    TOKEN_NUMERO,
    TOKEN_IDENTIFICADOR,
    TOKEN_PARENTESIS_IZQ,
    TOKEN_PARENTESIS_DER,
    TOKEN_LLAVE_IZQ,
    TOKEN_LLAVE_DER,
    TOKEN_PUNTO_COMA,
    TOKEN_COMA,
    TOKEN_ASIGNACION,
    TOKEN_EOF, // END OF FILE
    TOKEN_CADENA,
    TOKEN_DECIMAL,
    TOKEN_IGUALDAD,
    TOKEN_DESIGUALDAD,
    TOKEN_SI,
    TOKEN_NO,
    
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
    if (valor == "entero") return TOKEN_ENTERO;
    if (valor == "escribir") return TOKEN_ESCRIBIR;
    if (valor == "esperar") return TOKEN_ESPERAR;
    if (valor == "repetir") return TOKEN_REPETIR;
    if (valor == "veces") return TOKEN_VECES;
    if (valor == "configurar_pin") return TOKEN_CONFIGURAR_PIN;
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
                    std::cerr << "Error léxico: comentario sin terminar en línea " << linea << ", columna " << columna << std::endl;
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
                    std::cerr << "Error léxico: cadena sin terminar en línea " << linea << ", columna " << columna << std::endl;
                    break;
                }
                buffer += c;
                columna++;
            }
            if (c != '"') {
                std::cerr << "Error léxico: cadena sin terminar en línea " << linea << ", columna " << columna << std::endl;
            } else {
                tokens.push_back({TOKEN_CADENA, buffer, linea, columna - buffer.length()});
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
                    std::cerr << "Error léxico: número con múltiples puntos en línea " << linea << ", columna " << columna << std::endl;
                    break;
                }
                buffer += c;
                columna++;
            }
            archivo.unget(); // Devolver el último carácter leído

            if (buffer.find('.') != std::string::npos) {
                tokens.push_back({TOKEN_DECIMAL, buffer, linea, columna - buffer.length()});
            } else {
                tokens.push_back({TOKEN_NUMERO, buffer, linea, columna - buffer.length()});
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
            tokens.push_back({tipo, buffer, linea, columna - buffer.length()});
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
                    tokens.push_back({tipo, "==", linea, columna});
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
                    tokens.push_back({tipo, "!=", linea, columna});
                    columna += 2;
                    continue;
                } else {
                    std::cerr << "Error léxico: carácter inesperado '!' en línea " << linea << ", columna " << columna << std::endl;
                    columna++;
                    continue;
                }
                break;
            default:
                std::cerr << "Error léxico: carácter inesperado '" << c << "' en línea " << linea << ", columna " << columna << std::endl;
                columna++;
                continue;
        }
        tokens.push_back({tipo, std::string(1, c), linea, columna});
        columna++;
    }

    // Añadir token de fin de archivo
    tokens.push_back({TOKEN_EOF, "", linea, columna});

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
        std::cout << "Token: " << token.value << " (Tipo: " << token.type << ", Línea: " << token.line << ", Columna: " << token.column << ")" << std::endl;
    }

    return 0;
}