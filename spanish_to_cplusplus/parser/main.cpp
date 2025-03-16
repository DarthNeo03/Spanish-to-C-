#include "lexer.h"
#include "parser.h"

int main() {
    std::string ruta;
    std::cout << "Ingrese la ruta del archivo: ";
    std::getline(std::cin, ruta);

    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    try {
        auto tokens = analizadorLexico(archivo);
        Parser parser(tokens);
        parser.analizar();
        std::cout << "Análisis sintáctico completado exitosamente!" << std::endl;
    }
    catch (const ErrorSintactico& e) {
        std::cerr << "Error sintáctico: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
