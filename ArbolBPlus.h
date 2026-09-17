/**
 * Archivo: ArbolBPlus.h
 * Propósito: Define las estructuras fundamentales de la base de datos: el Registro (fila),
 *            el Nodo (hojas e internos) y la clase que gestiona la lógica del Árbol B+.
 */
#ifndef ARBOL_BPLUS_H
#define ARBOL_BPLUS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

struct Registro {
    int clave;
    string datos;
    string serializar() const;
};

struct NodoBPlus {
    bool es_hoja;
    vector<int> claves;
    vector<NodoBPlus*> hijos;
    vector<Registro> registros;
    NodoBPlus* siguiente_hoja;

    NodoBPlus(bool hoja);
};

class ArbolBPlus {
private:
    NodoBPlus* raiz;
    int grado;
    string nombre_archivo;

    void insertarInterno(int clave, NodoBPlus* cursor, NodoBPlus* hijo);
    NodoBPlus* buscarPadre(NodoBPlus* cursor, NodoBPlus* hijo);
    void liberarNodos();

public:
    ArbolBPlus(int _grado, string _nombre_archivo);
    ~ArbolBPlus();

    void insertar(int clave, string datos);
    string buscar(int clave);
    void eliminar(int clave);
    vector<Registro> obtenerTodos();

    void guardarEnArchivo();
    void cargarDesdeArchivo();

    // Limpia completamente el árbol que está en RAM.
    void limpiar();

    // Elimina el archivo físico asociado al árbol.
    void eliminarArchivo();
};

#endif // ARBOL_BPLUS_H
