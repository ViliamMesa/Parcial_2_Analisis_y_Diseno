/**
 * Archivo: ArbolBPlus.h
 * Propósito: Define las estructuras fundamentales de la base de datos: el Registro (fila),
 *            los nodos y la clase que implementa el Árbol B+ y su persistencia.
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

    // Inserta una clave separadora y el nuevo hijo derecho en un nodo interno.
    // La propagación de splits se controla desde insertar() usando el camino
    // recorrido desde la raíz hasta la hoja, evitando buscar el padre recorriendo
    // nuevamente todo el árbol.
    void insertarEnPadre(NodoBPlus* padre, int clave, NodoBPlus* hijoDerecho);

    // Libera todos los nodos del árbol actual.
    void liberarNodos();

public:
    ArbolBPlus(int _grado, string _nombre_archivo);
    ~ArbolBPlus();

    // Inserción ordenada en hojas, con split y propagación ascendente.
    void insertar(int clave, string datos);

    // Búsqueda desde la raíz hasta la hoja correspondiente.
    string buscar(int clave);

    // Eliminación básica. La versión de bonus con redistribución/merge
    // puede implementarse posteriormente sin afectar las operaciones principales.
    void eliminar(int clave);

    // Recorre las hojas usando siguiente_hoja.
    vector<Registro> obtenerTodos();

    // Persistencia secuencial de los registros almacenados en las hojas.
    void guardarEnArchivo();
    void cargarDesdeArchivo();

    // Limpia completamente el árbol en RAM.
    void limpiar();

    // Elimina el archivo físico asociado al árbol.
    void eliminarArchivo();
};

#endif // ARBOL_BPLUS_H
