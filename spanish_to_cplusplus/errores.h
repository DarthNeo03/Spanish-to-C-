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

inline void imprimirErrores(const std::vector<Error>& errores) {
    if (errores.empty()) return;

    // Bordes básicos con caracteres ASCII y color rojo
    std::cout << "\n\033[1;31m+---------------------+\033[0m\n"; // Borde superior en rojo
    std::cout << "\033[1;31m| Errores encontrados (" << errores.size() << ")\033[0m\n"; // Título en rojo
    std::cout << "\033[1;31m+--------+--------+--------------+---------------------------------------+\033[0m\n"; // Borde encabezado en rojo
    std::cout << "\033[1;31m| Linea  | Columna| Tipo         | Descripcion                           |\033[0m\n"; // Encabezados en rojo
    std::cout << "\033[1;31m+--------+--------+--------------+---------------------------------------+\033[0m\n"; // Borde encabezado inferior en rojo

    for (const auto& error : errores) {
        std::cout << "\033[0m| " << std::setw(6) << error.linea
                  << " | " << std::setw(6) << error.columna
                  << " | " << std::setw(12) << error.tipo
                  << " | " << std::setw(37) << error.mensaje << " |\n";
    }
    
    std::cout << "\033[1;31m+--------+--------+--------------+---------------------------------------+\033[0m\n"; // Borde inferior en rojo
}

#endif // ERRORES_H