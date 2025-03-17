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
private:
    std::unordered_map<std::string, Simbolo> simbolos;

public:
    void insertar(const Simbolo& simbolo) {
        simbolos[simbolo.nombre] = simbolo;
    }

    Simbolo* buscar(const std::string& nombre) {
        auto it = simbolos.find(nombre);
        return (it != simbolos.end()) ? &it->second : nullptr;
    }

    void limpiar() { simbolos.clear(); }
};

#endif // SIMBOLOS_H
