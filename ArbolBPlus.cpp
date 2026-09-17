/**
 * Archivo: ArbolBPlus.cpp
 * Propósito: Implementación del motor de almacenamiento basado en Árbol B+.
 */
#include "ArbolBPlus.h"
#include <cstdio>

using namespace std;

string Registro::serializar() const {
    return to_string(clave) + "," + datos;
}

NodoBPlus::NodoBPlus(bool hoja) {
    es_hoja = hoja;
    siguiente_hoja = nullptr;
}

ArbolBPlus::ArbolBPlus(int _grado, string _nombre_archivo)
    : raiz(nullptr), grado(_grado), nombre_archivo(_nombre_archivo) {}

ArbolBPlus::~ArbolBPlus() {
    liberarNodos();
}

void ArbolBPlus::liberarNodos() {
    if (raiz == nullptr) return;

    vector<NodoBPlus*> nodos;
    nodos.push_back(raiz);

    // Los nodos hoja también son hijos del árbol, por lo que no necesitamos
    // recorrer siguiente_hoja para liberarlos.
    for (size_t i = 0; i < nodos.size(); ++i) {
        if (!nodos[i]->es_hoja) {
            for (size_t j = 0; j < nodos[i]->hijos.size(); ++j)
                nodos.push_back(nodos[i]->hijos[j]);
        }
    }

    for (vector<NodoBPlus*>::reverse_iterator it = nodos.rbegin();
         it != nodos.rend(); ++it) {
        delete *it;
    }

    raiz = nullptr;
}

void ArbolBPlus::limpiar() {
    liberarNodos();
}

void ArbolBPlus::eliminarArchivo() {
    if (!nombre_archivo.empty())
        remove(nombre_archivo.c_str());
}

void ArbolBPlus::insertarEnPadre(NodoBPlus* padre, int clave, NodoBPlus* hijoDerecho) {
    size_t pos = 0;
    while (pos < padre->claves.size() && padre->claves[pos] < clave)
        ++pos;

    padre->claves.insert(padre->claves.begin() + pos, clave);
    padre->hijos.insert(padre->hijos.begin() + pos + 1, hijoDerecho);
}

void ArbolBPlus::insertar(int clave, string datos) {
    if (raiz == nullptr) {
        raiz = new NodoBPlus(true);
        raiz->registros.push_back({clave, datos});
        raiz->claves.push_back(clave);
        return;
    }

    // Guardamos los nodos internos recorridos. Así, cuando exista un split,
    // conocemos inmediatamente el padre y los siguientes ancestros.
    vector<NodoBPlus*> camino;
    NodoBPlus* cursor = raiz;

    while (!cursor->es_hoja) {
        camino.push_back(cursor);

        size_t i = 0;
        // Si la clave es igual al separador, seguimos el hijo derecho.
        while (i < cursor->claves.size() && clave >= cursor->claves[i])
            ++i;
        cursor = cursor->hijos[i];
    }

    // Insertar en la hoja manteniendo el orden.
    size_t pos = 0;
    while (pos < cursor->registros.size() && cursor->registros[pos].clave < clave)
        ++pos;

    // La clave primaria es única: si ya existe, actualizamos los datos.
    if (pos < cursor->registros.size() && cursor->registros[pos].clave == clave) {
        cursor->registros[pos].datos = datos;
        return;
    }

    cursor->registros.insert(cursor->registros.begin() + pos, {clave, datos});
    cursor->claves.clear();
    for (size_t i = 0; i < cursor->registros.size(); ++i)
        cursor->claves.push_back(cursor->registros[i].clave);

    // Caso normal: la hoja todavía tiene capacidad.
    if ((int)cursor->registros.size() <= grado)
        return;

    // ============================================================
    // SPLIT DE HOJA
    // ============================================================
    NodoBPlus* nuevaHoja = new NodoBPlus(true);
    size_t mitad = cursor->registros.size() / 2;

    nuevaHoja->registros.assign(cursor->registros.begin() + mitad,
                                cursor->registros.end());
    cursor->registros.erase(cursor->registros.begin() + mitad,
                            cursor->registros.end());

    cursor->claves.clear();
    nuevaHoja->claves.clear();

    for (size_t i = 0; i < cursor->registros.size(); ++i)
        cursor->claves.push_back(cursor->registros[i].clave);

    for (size_t i = 0; i < nuevaHoja->registros.size(); ++i)
        nuevaHoja->claves.push_back(nuevaHoja->registros[i].clave);

    // Mantener la lista enlazada de hojas para SELECT *.
    nuevaHoja->siguiente_hoja = cursor->siguiente_hoja;
    cursor->siguiente_hoja = nuevaHoja;

    // En una hoja la clave separadora se COPIA al padre, pero permanece
    // también en la hoja derecha porque allí está el registro real.
    int separadora = nuevaHoja->claves.front();

    if (camino.empty()) {
        // El split fue directamente sobre la raíz.
        NodoBPlus* nuevaRaiz = new NodoBPlus(false);
        nuevaRaiz->claves.push_back(separadora);
        nuevaRaiz->hijos.push_back(cursor);
        nuevaRaiz->hijos.push_back(nuevaHoja);
        raiz = nuevaRaiz;
        return;
    }

    // Insertamos el nuevo hijo y el separador en el padre inmediato.
    NodoBPlus* padre = camino.back();
    insertarEnPadre(padre, separadora, nuevaHoja);

    // ============================================================
    // PROPAGACIÓN ASCENDENTE DE SPLITS INTERNOS
    // ============================================================
    while ((int)padre->claves.size() > grado) {
        size_t medio = padre->claves.size() / 2;
        int promovida = padre->claves[medio];

        NodoBPlus* nuevoInterno = new NodoBPlus(false);

        // En un nodo interno la clave promovida se MUEVE al padre.
        // Por eso no queda almacenada en ninguno de los dos nodos resultantes.
        nuevoInterno->claves.assign(padre->claves.begin() + medio + 1,
                                    padre->claves.end());
        nuevoInterno->hijos.assign(padre->hijos.begin() + medio + 1,
                                   padre->hijos.end());

        padre->claves.erase(padre->claves.begin() + medio,
                            padre->claves.end());
        padre->hijos.erase(padre->hijos.begin() + medio + 1,
                           padre->hijos.end());

        // Si el nodo que se partió era la raíz, creamos una nueva raíz.
        if (camino.size() == 1) {
            NodoBPlus* nuevaRaiz = new NodoBPlus(false);
            nuevaRaiz->claves.push_back(promovida);
            nuevaRaiz->hijos.push_back(padre);
            nuevaRaiz->hijos.push_back(nuevoInterno);
            raiz = nuevaRaiz;
            return;
        }

        // Subimos un nivel del camino y continuamos propagando el split.
        camino.pop_back();
        NodoBPlus* abuelo = camino.back();
        insertarEnPadre(abuelo, promovida, nuevoInterno);
        padre = abuelo;
    }
}

string ArbolBPlus::buscar(int clave) {
    if (raiz == nullptr) return "";

    NodoBPlus* cursor = raiz;

    while (!cursor->es_hoja) {
        size_t i = 0;
        while (i < cursor->claves.size() && clave >= cursor->claves[i])
            ++i;
        cursor = cursor->hijos[i];
    }

    for (size_t i = 0; i < cursor->registros.size(); ++i) {
        if (cursor->registros[i].clave == clave)
            return cursor->registros[i].datos;
        if (cursor->registros[i].clave > clave)
            break;
    }

    return "";
}

void ArbolBPlus::eliminar(int clave) {
    if (raiz == nullptr) return;

    // Implementación base del DELETE. Mantiene el árbol correcto reconstruyendo
    // la estructura después de quitar el registro. El merge/redistribución de
    // underflow queda reservado para el bonus del parcial.
    vector<Registro> registros = obtenerTodos();

    size_t encontrada = registros.size();
    for (size_t i = 0; i < registros.size(); ++i) {
        if (registros[i].clave == clave) {
            encontrada = i;
            break;
        }
    }

    if (encontrada == registros.size()) return;
    registros.erase(registros.begin() + encontrada);

    limpiar();

    for (size_t i = 0; i < registros.size(); ++i)
        insertar(registros[i].clave, registros[i].datos);
}

vector<Registro> ArbolBPlus::obtenerTodos() {
    vector<Registro> resultado;
    if (raiz == nullptr) return resultado;

    // Buscamos la primera hoja y después recorremos toda la cadena enlazada.
    NodoBPlus* cursor = raiz;
    while (!cursor->es_hoja)
        cursor = cursor->hijos.front();

    while (cursor != nullptr) {
        for (size_t i = 0; i < cursor->registros.size(); ++i)
            resultado.push_back(cursor->registros[i]);
        cursor = cursor->siguiente_hoja;
    }

    return resultado;
}

void ArbolBPlus::guardarEnArchivo() {
    if (nombre_archivo.empty()) return;

    ofstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo " << nombre_archivo << endl;
        return;
    }

    vector<Registro> registros = obtenerTodos();
    for (size_t i = 0; i < registros.size(); ++i)
        archivo << registros[i].serializar() << '\n';
}

void ArbolBPlus::cargarDesdeArchivo() {
    if (nombre_archivo.empty()) return;

    ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) return;

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;

        size_t separador = linea.find(',');
        if (separador == string::npos) continue;

        try {
            int clave = stoi(linea.substr(0, separador));
            string datos = linea.substr(separador + 1);
            insertar(clave, datos);
        } catch (...) {
            cerr << "Registro invalido ignorado: " << linea << endl;
        }
    }
}
