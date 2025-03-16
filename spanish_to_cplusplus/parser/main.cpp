#include "lexer.h"
#include "parser.h"

int main() {
    std::string ruta;
    std::cout << "Ingrese la ruta del archivo: ";
    std::getline(std::cin, ruta);

    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return 1;
    }
    
    try {
        // Realizar el análisis léxico
        auto tokens = analizadorLexico(archivo);
        std::cout << "Analisis lexico completado!" << std::endl;
        // Imprimir los tokens reconocidos
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.value 
                    << " (Tipo: " << tokenTypeToString(token.type)
                    << ", Linea: " << token.line 
                    << ", Columna: " << token.column << ")" << std::endl;
        }

        // Realizar el análisis sintáctico
        Parser parser(tokens);
        parser.analizar();
        std::cout << "Análisis sintáctico completado exitosamente!" << std::endl;
    }
    catch (const ErrorSintactico& e) {
        std::cerr << "Error sintáctico: " << e.what() << std::endl;
        return 1;
    }

    // Cerrar el archivo
    archivo.close();    

    return 0;
}
