#ifndef SIMBOLOS_H
#define SIMBOLOS_H

#include <string>
#include <unordered_map>
#include <variant>

enum TipoDato {
    ENTERO,
    DECIMAL,
    CADENA,
    BOOLEANO,
    INDEFINIDO
};

struct Simbolo {
    std::string nombre;
    TipoDato tipo;
    int lineaDeclaracion;
    std::variant<int, float, std::string, bool> valor;
};

class TablaSimbolos {
public:
    std::unordered_map<std::string, Simbolo> simbolos;
    
    void insertar(const Simbolo& simbolo) {
        simbolos[simbolo.nombre] = simbolo;
    }

    Simbolo* buscar(const std::string& nombre) {
        auto it = simbolos.find(nombre);
        return (it != simbolos.end()) ? &it->second : nullptr;
    }

    void limpiar() { simbolos.clear(); }

    void agregar(const std::string& nombre, TipoDato tipo, int linea, const std::variant<int, float, std::string, bool>& valor) {
        simbolos[nombre] = {nombre, tipo, linea, valor}; // Asegurar almacenar valor real
    }
};

inline std::string tipoDatoToString(TipoDato tipo) {
    switch(tipo) {
        case ENTERO:    return "ENTERO";
        case DECIMAL:   return "DECIMAL";
        case CADENA:    return "CADENA";
        case BOOLEANO:  return "BOOLEANO";
        default:        return "INDEFINIDO";
    }
}

inline std::string valorToString(const std::variant<int, float, std::string, bool>& valor) {
    if (std::holds_alternative<int>(valor)) return std::to_string(std::get<int>(valor));
    if (std::holds_alternative<float>(valor)) return std::to_string(std::get<float>(valor));
    if (std::holds_alternative<std::string>(valor)) return std::get<std::string>(valor);
    if (std::holds_alternative<bool>(valor)) return std::get<bool>(valor) ? "true" : "false";
    return "N/A";
}
#endif // SIMBOLOS_H