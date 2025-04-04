#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "jsonParser.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

void imprimirTokens(const std::vector<Token>& tokens) {
    // Verificar si la lista de tokens está vacía
    if (tokens.empty()) return;

    /*for (const auto& token : tokens) {
            std::cout << "Token: " << token.value 
                    << " (Tipo: " << tokenTypeToString(token.type)
                    << ", Linea: " << token.line 
                    << ", Columna: " << token.column << ")" << std::endl;
        }*/

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


    // Generar JSON de tokens
    GeneradorJSON::generarJsonTokens(tokens, "tokens.json");
}


int main(int argc, char* argv[]) {
    std::vector<Error> erroresGlobales;
    std::string ruta;

    if (argc > 1) {
        ruta = argv[1];
    } else {
        std::cout << "Ingrese la ruta del archivo: ";
        std::getline(std::cin, ruta);
    }

    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return 1;
    }
    
    try {
        auto tokens = analizadorLexico(archivo, erroresGlobales);
        std::cout << "\n\033[1;34mTabla de Simbolos\033[0m\n";
        imprimirTokens(tokens);
        std::cout << "Analisis lexico completado!" << std::endl;
        std::cout << "Analisis lexico completado! \nIniciando analisis sintactico" << std::endl;

        Parser parser(tokens, erroresGlobales);
        auto ast = parser.analizar();

        if (erroresGlobales.empty()) {
            std::cout << "Analisis sintactico completado!" << std::endl;

            std::ofstream jsonAst("ast.json");
            jsonAst << parser.astToJson(ast) << std::endl;
            jsonAst.close();

            // Realizar el análisis semántico
            AnalizadorSemantico semantico(erroresGlobales, parser.obtenerTablaSimbolos());
            semantico.analizar(ast.get());
            
            // Guardar en archivo y manejar errores
            semantico.guardarEnArchivo("salida.cpp");
            
            if (erroresGlobales.empty()) {
                std::cout << "\n\033[1;32mCodigo generado exitosamente en salida.cpp!\033[0m\n";
            } else {
                ::imprimirErrores(erroresGlobales);
                std::cerr << "Error al guardar el archivo de salida." << std::endl;
            }
        } else {
            ::imprimirErrores(erroresGlobales);
            std::cerr << "Analisis completado con errores!" << std::endl;
            if (!erroresGlobales.empty()) {
                GeneradorJSON::generarJsonError(
                    erroresGlobales,
                    "errores.json"
                );
            }
        }   

         
    } catch (const std::bad_alloc&) {
        std::cerr << "\nERROR CRÍTICO: Memoria insuficiente. Verifique errores de bucle infinito\n";
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    archivo.close();    
    return 0;
}