#ifndef ERRORES_H
#define ERRORES_H

#include <vector>
#include <string>
#include <iomanip>
#include <iostream>

struct Error {
    std::string mensaje;
    int linea;
    int columna;
    std::string tipo; // "Léxico", "Sintáctico" o "Semántico"
};

// Función para imprimir errores en formato de tabla
inline void imprimirErrores(const std::vector<Error>& errores) {
    if (errores.empty()) return;

    std::cout << "\nErrores encontrados (" << errores.size() << "):\n";
    std::cout << std::left 
        << std::setw(8) << "Linea"
        << std::setw(8) << "Columna"
        << std::setw(12) << "Tipo"
        << "Descripcion\n";
    
    for (const auto& error : errores) {
        std::cout << std::setw(8) << error.linea
                << std::setw(8) << error.columna
                << std::setw(12) << error.tipo
                << error.mensaje << "\n";
    }
}

#endif // ERRORES_H