#include "lexer.h"
// #include "parser.h"

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
