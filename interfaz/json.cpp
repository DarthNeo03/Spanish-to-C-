#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp> // Debes instalar esta biblioteca: https://github.com/nlohmann/json

using json = nlohmann::json;

void generarJSON(const std::vector<std::string>& tokens, 
                 const std::vector<std::string>& tablaSimbolos,
                 const std::vector<std::string>& errores) {
    json j;

    j["tokens"] = tokens;
    j["tabla_simbolos"] = tablaSimbolos;
    j["errores"] = errores;

    std::ofstream archivo("salida.json");
    if (archivo.is_open()) {
        archivo << j.dump(4); // Formateado con indentación de 4 espacios
        archivo.close();
    } else {
        std::cerr << "Error al crear el archivo JSON" << std::endl;
    }
}

void generarCodigoCPP(const std::string& codigo) {
    std::ofstream archivo("salida.cpp");
    if (archivo.is_open()) {
        archivo << codigo;
        archivo.close();
    } else {
        std::cerr << "Error al crear el archivo de código" << std::endl;
    }
}

int main() {
    std::vector<std::string> tokens = {"INICIO", "VAR", "ASIGNACION"};
    std::vector<std::string> tablaSimbolos = {"a: int", "b: float"};
    std::vector<std::string> errores = {"Error en la línea 3: variable no declarada"};
    std::string codigoGenerado = "#include <Arduino.h>\n\nvoid setup() {}\nvoid loop() {}";

    generarJSON(tokens, tablaSimbolos, errores);
    generarCodigoCPP(codigoGenerado);

    std::cout << "Archivos generados correctamente." << std::endl;

    return 0;
}
