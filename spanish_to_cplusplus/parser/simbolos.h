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
    std::variant<int, float, std::string, bool> valor;
    int lineaDeclaracion;
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

    void limpiar() {
        simbolos.clear();
    }
};
