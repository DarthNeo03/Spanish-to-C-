#ifndef JSON_H
#define JSON_H

#include "simbolos.h"
#include "errores.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>
#include <stdexcept>

using json = nlohmann::json;

class GeneradorJSON
{
public:
    inline static void generarJsonError(const std::vector<Error> &errores, const std::string &archivoSalida = "errores.json")
    {
        json resultado;

        // Serializar errores
        for (const auto &error : errores)
        {
            json entrada = {
                {"mensaje", error.mensaje},
                {"linea", error.linea},
                {"columna", error.columna},
                {"tipo", error.tipo}};
            resultado["tablaErrores"].push_back(entrada);
        }

        // Escribir archivo
        std::ofstream archivo(archivoSalida);
        if (archivo.is_open())
        {
            archivo << resultado.dump(4);
            std::cout << "\nJSON generado: " << archivoSalida << std::endl;
        }
        else
        {
            std::cerr << "Error generando archivo JSON: " + archivoSalida << std::endl;
        }
    };
    inline static void generarJsonSimbolos(TablaSimbolos& symbols, const std::string &archivoSalida = "simbolos.json")
    {
        json resultado;

        // Serializar tabla de símbolos
        for (const auto& [nombre, simbolo] : symbols.simbolos) {
            json entrada = {
                {"nombre", nombre},
                {"tipo", tipoDatoToString(simbolo.tipo)},
                {"linea", simbolo.lineaDeclaracion},
                {"valor", simbolo.valor}};
            resultado["tablaSimbolos"].push_back(entrada);
        }

        // Escribir archivo
        std::ofstream archivo(archivoSalida);
        if (archivo.is_open())
        {
            archivo << resultado.dump(4);
            std::cout << "\nJSON generado: " << archivoSalida << std::endl;
        }
        else
        {
            std::cerr << "Error generando archivo JSON: " + archivoSalida << std::endl;
        }
    };
    inline static void generarJsonTokens(const std::vector<Token>& tokens, const std::string &archivoSalida = "tokens.json")
    {
        json resultado;

        // Serializar tabla de tokens
        for (const auto& token : tokens) {
            json entrada = {
                {"token", token.value},
                {"tipo", tokenTypeToString(token.type)},
                {"linea", token.line},
                {"columna", token.column}};
            resultado["tablaTokens"].push_back(entrada);
        }

        // Escribir archivo
        std::ofstream archivo(archivoSalida);
        if (archivo.is_open())
        {
            archivo << resultado.dump(4);
            std::cout << "\nJSON generado: " << archivoSalida << std::endl;
        }
        else
        {
            std::cerr << "Error generando archivo JSON: " + archivoSalida << std::endl;
        }
    };
};
#endif // JSON_H