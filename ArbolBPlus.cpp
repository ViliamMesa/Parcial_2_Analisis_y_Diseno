/**
 * Archivo: ArbolBPlus.cpp
 * Propósito: Implementación del motor de almacenamiento basado en Árbol B+.
 */
#include "ArbolBPlus.h"
#include <algorithm>
#include <sstream>
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

    for (size_t i = 0; i < nodos.size(); ++i) {
        if (!nodos[i]->es_hoja) {
            for (size_t j = 0; j < nodos[i]->hijos.size(); ++j)
                nodos.push_back(nodos[i]->hijos[j]);
        }
    }

    for (vector<NodoBPlus*>::reverse_iterator it = nodos.rbegin(); it != nodos.rend(); ++it)
        delete *it;

    raiz = nullptr;
}

void ArbolBPlus::limpiar() {
    liberarNodos();
}

void ArbolBPlus::eliminarArchivo() {
    remove(nombre_archivo.c_str());
}

NodoBPlus* ArbolBPlus::buscarPadre(NodoBPlus* cursor, NodoBPlus* hijo) {
    if (cursor == nullptr || cursor->es_hoja) return nullptr;

    for (size_t i = 0; i < cursor->hijos.size(); ++i) {
        if (cursor->hijos[i] == hijo) return cursor;
    }

    for (size_t i = 0; i < cursor->hijos.size(); ++i) {
        NodoBPlus* padre = buscarPadre(cursor->hijos[i], hijo);
        if (padre != nullptr) return padre;
    }
    return nullptr;
}

void ArbolBPlus::insertarInterno(int clave, NodoBPlus* cursor, NodoBPlus* hijo) {
    if (cursor == nullptr) return;

    size_t pos = 0;
    while (pos < cursor->claves.size() && cursor->claves[pos] < clave) ++pos;
    cursor->claves.insert(cursor->claves.begin() + pos, clave);
    cursor->hijos.insert(cursor->hijos.begin() + pos + 1, hijo);

    if ((int)cursor->claves.size() <= grado) return;

    int medio = grado / 2;
    int promovida = cursor->claves[medio];
    NodoBPlus* nuevo = new NodoBPlus(false);

    nuevo->claves.assign(cursor->claves.begin() + medio + 1, cursor->claves.end());
    nuevo->hijos.assign(cursor->hijos.begin() + medio + 1, cursor->hijos.end());

    cursor->claves.erase(cursor->claves.begin() + medio, cursor->claves.end());
    cursor->hijos.erase(cursor->hijos.begin() + medio + 1, cursor->hijos.end());

    if (cursor == raiz) {
        NodoBPlus* nuevaRaiz = new NodoBPlus(false);
        nuevaRaiz->claves.push_back(promovida);
        nuevaRaiz->hijos.push_back(cursor);
        nuevaRaiz->hijos.push_back(nuevo);
        raiz = nuevaRaiz;
    } else {
        insertarInterno(promovida, buscarPadre(raiz, cursor), nuevo);
    }
}

void ArbolBPlus::insertar(int clave, string datos) {
    if (raiz == nullptr) {
        raiz = new NodoBPlus(true);
        raiz->registros.push_back({clave, datos});
        raiz->claves.push_back(clave);
        return;
    }

    NodoBPlus* cursor = raiz;
    while (!cursor->es_hoja) {
        size_t i = 0;
        while (i < cursor->claves.size() && clave >= cursor->claves[i]) ++i;
        cursor = cursor->hijos[i];
    }

    size_t pos = 0;
    while (pos < cursor->registros.size() && cursor->registros[pos].clave < clave) ++pos;

    if (pos < cursor->registros.size() && cursor->registros[pos].clave == clave) {
        cursor->registros[pos].datos = datos;
        return;
    }

    cursor->registros.insert(cursor->registros.begin() + pos, {clave, datos});
    cursor->claves.clear();
    for (size_t i = 0; i < cursor->registros.size(); ++i)
        cursor->claves.push_back(cursor->registros[i].clave);

    if ((int)cursor->registros.size() <= grado) return;

    NodoBPlus* nuevaHoja = new NodoBPlus(true);
    size_t mitad = cursor->registros.size() / 2;

    nuevaHoja->registros.assign(cursor->registros.begin() + mitad, cursor->registros.end());
    cursor->registros.erase(cursor->registros.begin() + mitad, cursor->registros.end());

    cursor->claves.clear();
    nuevaHoja->claves.clear();
    for (size_t i = 0; i < cursor->registros.size(); ++i)
        cursor->claves.push_back(cursor->registros[i].clave);
    for (size_t i = 0; i < nuevaHoja->registros.size(); ++i)
        nuevaHoja->claves.push_back(nuevaHoja->registros[i].clave);

    nuevaHoja->siguiente_hoja = cursor->siguiente_hoja;
    cursor->siguiente_hoja = nuevaHoja;

    int separadora = nuevaHoja->claves.front();

    if (cursor == raiz) {
        NodoBPlus* nuevaRaiz = new NodoBPlus(false);
        nuevaRaiz->claves.push_back(separadora);
        nuevaRaiz->hijos.push_back(cursor);
        nuevaRaiz->hijos.push_back(nuevaHoja);
        raiz = nuevaRaiz;
    } else {
        insertarInterno(separadora, buscarPadre(raiz, cursor), nuevaHoja);
    }
}

string ArbolBPlus::buscar(int clave) {
    if (raiz == nullptr) return "";

    NodoBPlus* cursor = raiz;
    while (!cursor->es_hoja) {
        size_t i = 0;
        while (i < cursor->claves.size() && clave >= cursor->claves[i]) ++i;
        cursor = cursor->hijos[i];
    }

    for (size_t i = 0; i < cursor->registros.size(); ++i) {
        if (cursor->registros[i].clave == clave) return cursor->registros[i].datos;
        if (cursor->registros[i].clave > clave) break;
    }
    return "";
}

void ArbolBPlus::eliminar(int clave) {
    if (raiz == nullptr) return;

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

    liberarNodos();

    for (size_t i = 0; i < registros.size(); ++i)
        insertar(registros[i].clave, registros[i].datos);
}

vector<Registro> ArbolBPlus::obtenerTodos() {
    vector<Registro> resultado;
    if (raiz == nullptr) return resultado;

    NodoBPlus* cursor = raiz;
    while (!cursor->es_hoja) cursor = cursor->hijos.front();

    while (cursor != nullptr) {
        for (size_t i = 0; i < cursor->registros.size(); ++i)
            resultado.push_back(cursor->registros[i]);
        cursor = cursor->siguiente_hoja;
    }
    return resultado;
}

void ArbolBPlus::guardarEnArchivo() {
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
