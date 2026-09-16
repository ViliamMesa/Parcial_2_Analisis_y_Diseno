/**
 * Archivo: ArbolBPlus.h
 * Propósito: Define las estructuras fundamentales de la base de datos: el Registro (fila),
 *            el Nodo (hojas e internos) y la clase que gestiona la lógica del Árbol B+.
 *            Aquí solo están las firmas (declaraciones), la implementación está en el .cpp.
 */
#ifndef ARBOL_BPLUS_H
#define ARBOL_BPLUS_H

#include <iostream> // Para imprimir mensajes de depuracion y errores (cout, cerr)
#include <string>   // Para manejar los datos de las filas como cadenas de texto
#include <vector>   // Para crear arrays dinamicos (listas de claves, hijos y registros)
#include <fstream>  // Para el manejo de archivos en disco (lectura/escritura de persistencia)

using namespace std;

/**
 * @struct Registro
 * Representa la unidad mínima de información almacenada, equivalente a una fila en SQL.
 */
struct Registro {
    int clave;    // Clave primaria (ej. ID numérico) - se usa para ordenar el árbol
    string datos; // Datos de la fila (simulando columnas en un string delimitado)

    // Convierte el registro en un string de una sola línea para guardarlo en un archivo plano.
    // Ejemplo: clave=1, datos="Juan" -> Retorna: "1,Juan"
    string serializar() const;
};

/**
 * @struct NodoBPlus
 * Representa un nodo genérico dentro del árbol B+.
 * En un Árbol B+, los nodos internos solo guardan "claves" y "punteros a hijos".
 * Los nodos hoja son los únicos que guardan los "registros" reales.
 */
struct NodoBPlus {
    bool es_hoja;                    // True si es nodo hoja (contiene registros reales)
    vector<int> claves;              // Claves de guía/búsqueda (útil para hojas e internos)
    vector<NodoBPlus*> hijos;        // Punteros a los nodos hijos (solo usado si es nodo interno)
    vector<Registro> registros;      // Datos reales (solo usado si es nodo hoja)
    
    // Puntero vital para los árboles B+: Conecta todas las hojas como una lista enlazada.
    // Permite hacer búsquedas secuenciales muy rápidas (SELECT *).
    NodoBPlus* siguiente_hoja;       

    // Constructor que inicializa el nodo
    NodoBPlus(bool hoja);
};

/**
 * @class ArbolBPlus
 * Maneja todas las operaciones del árbol y su persistencia en el disco.
 */
class ArbolBPlus {
private:
    NodoBPlus* raiz;        // Puntero a la raíz del árbol
    int grado;              // Máximo número de claves/hijos por nodo (para disparar splits)
    string nombre_archivo;  // Nombre del archivo .txt para guardar y cargar datos

    // Funciones auxiliares sugeridas para el estudiante que facilitan la recursión
    void insertarInterno(int clave, NodoBPlus* cursor, NodoBPlus* hijo);
    NodoBPlus* buscarPadre(NodoBPlus* cursor, NodoBPlus* hijo);

public:
    // Constructor de la base de datos
    ArbolBPlus(int _grado, string _nombre_archivo);

    // ==========================================
    // MÉTODOS A IMPLEMENTAR PARA EL PARCIAL
    // ==========================================
    
    // Agrega un nuevo registro al árbol manteniendo las reglas (orden y balanceo)
    void insertar(int clave, string datos);
    
    // Retorna los datos asociados a una clave (para búsquedas directas O(log n))
    string buscar(int clave);
    
    // Elimina un registro, puede implicar el merge (fusión) de nodos si quedan vacíos
    void eliminar(int clave);
    
    // Devuelve todos los registros, aprovechando el puntero "siguiente_hoja" de los nodos hoja
    vector<Registro> obtenerTodos();

    // ==========================================
    // MÉTODOS DE PERSISTENCIA (ARCHIVO DE TEXTO)
    // ==========================================
    
    // Escribe todas las hojas del árbol en un archivo
    void guardarEnArchivo();
    
    // Lee un archivo existente y reconstruye el árbol usando "insertar()"
    void cargarDesdeArchivo();
};

#endif // ARBOL_BPLUS_H
