#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "parser.h"
#include "simbolos.h"
#include <sstream>
#include <fstream>  // Para manejo de archivos
#include <vector>   // Para std::vector

class AnalizadorSemantico {
private:
    std::vector<Error>& errores;
    TablaSimbolos& tablaSimbolos;
    std::ostringstream codigoIntermedio;
    
    void generarCodigo(Nodo* nodo) {
        switch(nodo->tipo) {
            case NODO_PROGRAMA:
                generarPrograma(static_cast<NodoPrograma*>(nodo));
                break;
            case NODO_DECLARACION:
                generarDeclaracion(static_cast<NodoDeclaracion*>(nodo));
                break;
            case NODO_CONFIGURAR:
                generarConfigurar(static_cast<NodoConfigurar*>(nodo));
                break;
            case NODO_BUCLE_PRINCIPAL:
                generarBuclePrincipal(static_cast<NodoBuclePrincipal*>(nodo));
                break;
            case NODO_LLAMADA_FUNCION:
                generarLlamadaFuncion(static_cast<NodoLlamadaFuncion*>(nodo));
                break;
        }
    }

  void generarPrograma(NodoPrograma* programa) {
    // Generar declaraciones globales primero
    for (auto& decl : programa->declaraciones) {
        if (decl->tipo == NODO_DECLARACION) {
            generarCodigo(decl.get());
        }
    }
    
    codigoIntermedio << "\nvoid setup() {\n";
    for (auto& decl : programa->declaraciones) {
        if (decl->tipo == NODO_CONFIGURAR) {
            generarCodigo(decl.get());
        }
    }
    codigoIntermedio << "}\n\n";

    codigoIntermedio << "void loop() {\n";
    for (auto& decl : programa->declaraciones) {
        if (decl->tipo == NODO_BUCLE_PRINCIPAL) {
            generarCodigo(decl.get());
        }
    }
    codigoIntermedio << "}\n";
}

void generarDeclaracion(NodoDeclaracion* decl) {
    std::string tipoCpp;
    std::string valorTraducido = "";
    
    switch(decl->tipoDeclarado) {
        case ENTERO: 
            tipoCpp = "int"; 
            break;
        case DECIMAL: 
            tipoCpp = "float"; 
            break;
        case CADENA: 
            tipoCpp = "String";
            // Traducir valores especiales
            if (decl->expresion) {
                if (auto lit = dynamic_cast<NodoLiteral*>(decl->expresion.get())) {
                    std::string valor = lit->valor;
                    if (valor == "SALIDA") {
                        tipoCpp = "int";
                        valorTraducido = "OUTPUT";
                    } 
                    else if (valor == "ENTRADA") {
                        tipoCpp = "int";
                        valorTraducido = "INPUT";
                    }
                    else if (valor == "ALTO") { 
                        tipoCpp = "int";
                        valorTraducido = "HIGH";
                    }
                    else if (valor == "BAJO") {
                        tipoCpp = "int";
                        valorTraducido = "LOW";
                    }
                }
            }
            break;
        case BOOLEANO: 
            tipoCpp = "bool"; 
            break;
        default: 
            tipoCpp = "auto";
    }
    
    codigoIntermedio << tipoCpp << " " << decl->identificador;
    
    if (!valorTraducido.empty()) {
        codigoIntermedio << " = " << valorTraducido;
    } else if (decl->expresion) {
        codigoIntermedio << " = ";
        generarExpresion(decl->expresion.get());
    }
    
    codigoIntermedio << ";\n";
}

    void generarExpresion(NodoExpresion* expr) {
        if (auto var = dynamic_cast<NodoVariable*>(expr)) {
            codigoIntermedio << var->nombre;
        } else if (auto lit = dynamic_cast<NodoLiteral*>(expr)) {
            if (lit->tipoDato == "CADENA") {
                codigoIntermedio << "\"" << lit->valor << "\"";
            } else {
                codigoIntermedio << lit->valor;
            }
        }
    }

    void generarConfigurar(NodoConfigurar* config) {
        for (auto& instr : config->instrucciones) {
            generarCodigo(instr.get());
        }
    }

    void generarBuclePrincipal(NodoBuclePrincipal* bucle) {
        for (auto& instr : bucle->instrucciones) {
            generarCodigo(instr.get());
        }
    }

 void generarLlamadaFuncion(NodoLlamadaFuncion* llamada) {
    if (llamada->nombre == "configurar_pin") {
        codigoIntermedio << "pinMode(";
    } else if (llamada->nombre == "escribir") {
        codigoIntermedio << "digitalWrite(";
    } else if (llamada->nombre == "esperar") {
        codigoIntermedio << "delay(";
    }

    for (size_t i = 0; i < llamada->argumentos.size(); ++i) {
        if (auto lit = dynamic_cast<NodoLiteral*>(llamada->argumentos[i].get())) {
            if (lit->tipoDato == "CADENA") {
                std::string valor = lit->valor;
                // Traducir valores simbólicos de Arduino
                if (valor == "SALIDA") codigoIntermedio << "OUTPUT";
                else if (valor == "ALTO") codigoIntermedio << "HIGH";
                else if (valor == "BAJO") codigoIntermedio << "LOW";
                else codigoIntermedio << "\"" << valor << "\"";
            } else {
                codigoIntermedio << lit->valor;
            }
        } else {
            generarExpresion(llamada->argumentos[i].get());
        }
        
        if (i != llamada->argumentos.size() - 1) codigoIntermedio << ", ";
    }
    
    codigoIntermedio << ");\n";
}

public:
    AnalizadorSemantico(std::vector<Error>& err, TablaSimbolos& ts) 
        : errores(err), tablaSimbolos(ts) {}

    void analizar(NodoPrograma* programa) {
        codigoIntermedio << "#include <Arduino.h>\n\n";
        generarCodigo(programa);
    }

    void guardarEnArchivo(const std::string& nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        if (archivo.is_open()) {
            archivo << codigoIntermedio.str();
            archivo.close();
        } else {
            errores.push_back(Error{"No se pudo crear el archivo: " + nombreArchivo, -1});
        }
    }

    std::string obtenerCodigo() const {
        return codigoIntermedio.str();
    }
};

#endif