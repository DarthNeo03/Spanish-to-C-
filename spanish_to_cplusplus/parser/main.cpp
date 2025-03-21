#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <fstream>
#include <vector>

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
        auto tokens = analizadorLexico(archivo);
        // Imprimir los tokens reconocidos
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.value 
                    << " (Tipo: " << tokenTypeToString(token.type)
                    << ", Linea: " << token.line 
                    << ", Columna: " << token.column << ")" << std::endl;
        }
        std::cout << "Analisis lexico completado!" << std::endl;

        // Realizar el análisis sintáctico
        Parser parser(tokens, erroresGlobales);
        auto ast = parser.analizar();

        
    } catch (const std::bad_alloc&) {
        std::cerr << "\nERROR CRÍTICO: Memoria insuficiente. Verifique errores de bucle infinito\n";
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    imprimirErrores(erroresGlobales);
    if (erroresGlobales.empty()) {
        std::cout << "Analisis sintactico completado!" << std::endl;
        //parser.imprimirTablaSimbolos();
    }

    // Cerrar el archivo
    archivo.close();    

    return 0;
}
