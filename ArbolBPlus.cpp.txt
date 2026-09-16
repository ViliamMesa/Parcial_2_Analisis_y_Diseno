/**
 * Archivo: ArbolBPlus.cpp
 * Propósito: Aquí se implementan los métodos declarados en ArbolBPlus.h. 
 *            Esta es la clase central que los estudiantes deberán completar 
 *            como parte de su reto de Estructuras de Datos.
 */
#include "ArbolBPlus.h"

// -----------------------------------------------------------------------------
// Implementaciones de Estructuras Auxiliares
// -----------------------------------------------------------------------------

string Registro::serializar() const {
    // Ejemplo de salida: "1,Juan Perez" (clave + delimitador + datos)
    return to_string(clave) + "," + datos;
}

NodoBPlus::NodoBPlus(bool hoja) {
    // Por defecto al nacer, sabemos si es hoja o interno, pero no tiene nodos adyacentes aún.
    es_hoja = hoja;
    siguiente_hoja = nullptr;
}

// -----------------------------------------------------------------------------
// Constructor del Arbol B+
// -----------------------------------------------------------------------------

ArbolBPlus::ArbolBPlus(int _grado, string _nombre_archivo) : raiz(nullptr), grado(_grado), nombre_archivo(_nombre_archivo) {}


// =========================================================================
// MÉTODOS A IMPLEMENTAR PARA EL PARCIAL
// =========================================================================

void ArbolBPlus::insertar(int clave, string datos) {
    // [A IMPLEMENTAR EN EL PARCIAL]:
    // Lógica requerida:
    // 1. Si el árbol está vacío (raiz == nullptr), crear el primer nodo hoja.
    // 2. Si no está vacío, recorrer el árbol desde la raíz bajando por los hijos 
    //    correctos comparando la clave, hasta llegar a una hoja.
    // 3. Insertar el 'Registro' en el vector de registros de la hoja, MANTENIENDO EL ORDEN.
    // 4. Verificar condición de llenado: Si la hoja ahora tiene más elementos que el grado 
    //    (se desbordó), se debe dividir (SPLIT).
    // 5. El Split implica:
    //    a) Crear una nueva hoja.
    //    b) Pasar la mitad de los registros a la nueva hoja.
    //    c) Promover la clave media hacia el nodo PADRE.
    //    d) Configurar el puntero "siguiente_hoja" para mantener la lista enlazada unida.
    // 6. Esta propagación puede subir recursivamente hasta la raíz, obligando a crear una nueva raíz si es necesario.
    
    cout << "[Arbol B+] Insertando clave " << clave << " con dato: " << datos << " (NO IMPLEMENTADO)\n";
}

string ArbolBPlus::buscar(int clave) {
    // [A IMPLEMENTAR EN EL PARCIAL]:
    // Lógica requerida:
    // 1. Si la raíz es nullptr, devolver string vacío (no hay datos).
    // 2. Empezar en la raíz y hacer una búsqueda binaria o lineal sobre 'claves'.
    // 3. Si la clave buscada es menor que claves[i], bajar por hijos[i].
    // 4. Si la clave es mayor o igual, seguir iterando o bajar por el último hijo.
    // 5. Al llegar a un nodo hoja (`es_hoja == true`), buscar el registro exacto.
    // 6. Si se encuentra, retornar `registro.datos`, de lo contrario retornar string vacío.
    
    cout << "[Arbol B+] Buscando clave " << clave << " (NO IMPLEMENTADO)\n";
    return ""; // Retornar cadena vacía temporalmente para que compile
}

void ArbolBPlus::eliminar(int clave) {
    // [A IMPLEMENTAR EN EL PARCIAL]:
    // Lógica requerida:
    // 1. Localizar la hoja donde reside la clave.
    // 2. Eliminar el registro del vector.
    // 3. Verificar condición de 'underflow' (menos registros de los requeridos por el grado).
    // 4. Si hay underflow, intentar pedir prestado un registro a un nodo hermano (redistribución).
    // 5. Si no se puede pedir prestado, hacer 'merge' (fusión) con el hermano, 
    //    y eliminar la clave divisora en el nodo padre.
    
    cout << "[Arbol B+] Eliminando clave " << clave << " (NO IMPLEMENTADO)\n";
}

vector<Registro> ArbolBPlus::obtenerTodos() {
    vector<Registro> resultado;
    
    // [A IMPLEMENTAR EN EL PARCIAL]:
    // Lógica requerida:
    // 1. Bajar desde la raíz usando siempre el hijo[0] hasta llegar a la primera hoja (la más a la izquierda).
    // 2. Recorrer los registros de esa hoja e insertarlos en 'resultado'.
    // 3. Usar el puntero 'siguiente_hoja' para saltar a la próxima hoja.
    // 4. Repetir hasta que 'siguiente_hoja' sea nullptr.
    // Esto simula un comportamiento O(n) extremadamente rápido típico de las bases de datos (Full Table Scan).
    
    cout << "[Arbol B+] Escaneando todos los registros secuencialmente (NO IMPLEMENTADO)\n";
    return resultado;
}

// =========================================================================
// MÉTODOS DE PERSISTENCIA (ARCHIVO DE TEXTO)
// =========================================================================

void ArbolBPlus::guardarEnArchivo() {
    ofstream archivo(nombre_archivo);
    
    // Si no tenemos permisos o la ruta falla, abortamos
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo " << nombre_archivo << endl;
        return;
    }
    
    // [A IMPLEMENTAR EN EL PARCIAL]:
    // 1. Invocar 'obtenerTodos()' o hacer el recorrido manual de hojas.
    // 2. Por cada registro obtenido, llamar a 'registro.serializar()' y escribir esa cadena en el archivo.
    // 3. Añadir un salto de línea (endl) por cada registro.
    
    cout << "[Persistencia] Guardando datos en " << nombre_archivo << " (NO IMPLEMENTADO)\n";
    archivo.close();
}

void ArbolBPlus::cargarDesdeArchivo() {
    ifstream archivo(nombre_archivo);
    
    // Si el archivo no existe (ej. es la primera vez que corre el programa), ignorar sin error grave.
    if (!archivo.is_open()) {
        cout << "No existe archivo previo '" << nombre_archivo << "'. Se creará al guardar.\n";
        return;
    }
    
    // [A IMPLEMENTAR EN EL PARCIAL]:
    // 1. Leer línea por línea usando `getline(archivo, linea)`.
    // 2. Partir/Separar (Split) el string basándose en la coma ','.
    // 3. Convertir la primera parte a entero (ID).
    // 4. Pasar la segunda parte como string (Datos).
    // 5. Llamar al método `insertar(id, datos)` del mismo árbol B+ para poblarlo en memoria RAM.
    
    cout << "[Persistencia] Cargando datos desde " << nombre_archivo << " (NO IMPLEMENTADO)\n";
    archivo.close();
}
