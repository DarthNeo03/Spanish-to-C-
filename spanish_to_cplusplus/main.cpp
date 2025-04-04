#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

void imprimirTokens(const std::vector<Token>& tokens) {
    if (tokens.empty()) return;

    // Definir anchos de columna
    const int ANCHO_TOKEN = 18;
    const int ANCHO_TIPO = 21;
    const int ANCHO_LINEA = 6;
    const int ANCHO_COLUMNA = 8;

    // Bordes de la tabla en azul
    std::cout << "\033[1;34m" << std::left
              << "\n+" << std::string(ANCHO_TOKEN + 1, '-') 
              << "+" << std::string(ANCHO_TIPO + 1, '-')
              << "+" << std::string(ANCHO_LINEA + 1, '-')
              << "+" << std::string(ANCHO_COLUMNA + 1, '-') << "+\033[0m\n";

    // Cabecera en azul
    std::cout << "\033[1;34m| " << std::setw(ANCHO_TOKEN) << "Token"
              << "| " << std::setw(ANCHO_TIPO) << "Tipo"
              << "| " << std::setw(ANCHO_LINEA) << "Linea"
              << "| " << std::setw(ANCHO_COLUMNA) << "Columna" << "|\033[0m\n";

    // Separador en azul
    std::cout << "\033[1;34m+" << std::string(ANCHO_TOKEN + 1, '-') 
              << "+" << std::string(ANCHO_TIPO + 1, '-')
              << "+" << std::string(ANCHO_LINEA + 1, '-')
              << "+" << std::string(ANCHO_COLUMNA + 1, '-') << "+\033[0m\n";

    // Filas de datos
    for (const auto& token : tokens) {
        std::cout << "| " << std::setw(ANCHO_TOKEN) << token.value.substr(0, ANCHO_TOKEN)
                  << "| " << std::setw(ANCHO_TIPO) << tokenTypeToString(token.type)
                  << "| " << std::setw(ANCHO_LINEA) << token.line
                  << "| " << std::setw(ANCHO_COLUMNA) << token.column << "|\n";
    }

    // Bordes inferiores en azul
    std::cout << "\033[1;34m+" << std::string(ANCHO_TOKEN + 1, '-') 
              << "+" << std::string(ANCHO_TIPO + 1, '-')
              << "+" << std::string(ANCHO_LINEA + 1, '-')
              << "+" << std::string(ANCHO_COLUMNA + 1, '-') << "+\033[0m\n";
}


int main(int argc, char* argv[]) {
    std::vector<Error> erroresGlobales;
    std::string ruta;

    // Verificar si se proporcionó un argumento (ruta del archivo)
    if (argc > 1) {
        ruta = argv[1]; // Usar el primer argumento como ruta
    } else {
        // Solicitar la ruta al usuario si no se proporcionó como argumento
        std::cout << "Ingrese la ruta del archivo: ";
        std::getline(std::cin, ruta);
    }
    // Intentar abrir el archivo
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return 1;
    }
    
    try {
        // Realizar el análisis léxico
        auto tokens = analizadorLexico(archivo, erroresGlobales);
        // Imprimir los tokens reconocidos
        std::cout << "\n\033[1;34mTabla de Simbolos\033[0m\n";
        imprimirTokens(tokens);
        /*for (const auto& token : tokens) {
            std::cout << "Token: " << token.value 
                    << " (Tipo: " << tokenTypeToString(token.type)
                    << ", Linea: " << token.line 
                    << ", Columna: " << token.column << ")" << std::endl;
        }*/
        std::cout << "Analisis lexico completado!" << std::endl;

        // Realizar el análisis sintáctico
        Parser parser(tokens, erroresGlobales);
        auto ast = parser.analizar();

        if (erroresGlobales.empty()) {
            std::cout << "Analisis sintactico completado!" << std::endl;
            //parser.imprimirTablaSimbolos();

            AnalizadorSemantico semantico(erroresGlobales, parser.obtenerTablaSimbolos());
            semantico.analizar(ast.get());
            
            std::cout << "\n\033[1;34mSALIDA!\033[0m\n";
            std::cout << "\nCodigo generado:\n";
            std::cout << semantico.obtenerCodigo() << "\n";
        } else {
            imprimirErrores(erroresGlobales);
            std::cerr << "Analisis sintactico completado con errores!" << std::endl;
        }   

         
    } catch (const std::bad_alloc&) {
        std::cerr << "\nERROR CRÍTICO: Memoria insuficiente. Verifique errores de bucle infinito\n";
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Cerrar el archivo
    archivo.close();    

    return 0;
}
