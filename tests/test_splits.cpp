#include "../ArbolBPlus.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

static void comprobarBusqueda(ArbolBPlus& arbol, const vector<int>& claves) {
    for (size_t i = 0; i < claves.size(); ++i) {
        string esperado = "dato_" + to_string(claves[i]);
        assert(arbol.buscar(claves[i]) == esperado);
    }
}

static void comprobarOrden(ArbolBPlus& arbol, const vector<int>& esperadas) {
    vector<Registro> registros = arbol.obtenerTodos();
    assert(registros.size() == esperadas.size());

    for (size_t i = 0; i < esperadas.size(); ++i) {
        assert(registros[i].clave == esperadas[i]);
        assert(registros[i].datos == "dato_" + to_string(esperadas[i]));
    }
}

int main() {
    const string archivo = "test_splits_base.txt";

    // Grado 3: un nodo puede tener hasta 3 claves. La cuarta clave
    // provoca overflow y, por tanto, un split.
    ArbolBPlus arbol(3, archivo);

    // ------------------------------------------------------------
    // PRUEBA 1: split de hoja y creación de la primera raíz interna.
    // 10,20,30 llenan la hoja; 40 provoca el primer split.
    // ------------------------------------------------------------
    vector<int> claves;
    claves.push_back(10);
    claves.push_back(20);
    claves.push_back(30);
    claves.push_back(40);

    for (size_t i = 0; i < claves.size(); ++i)
        arbol.insertar(claves[i], "dato_" + to_string(claves[i]));

    comprobarBusqueda(arbol, claves);
    comprobarOrden(arbol, claves);

    // ------------------------------------------------------------
    // PRUEBA 2: varios splits de hojas dentro de la misma raíz.
    // Esto obliga a insertar separadores nuevos en el nodo interno.
    // ------------------------------------------------------------
    const int masClaves[] = {50, 60, 70, 80};
    for (size_t i = 0; i < sizeof(masClaves) / sizeof(masClaves[0]); ++i) {
        arbol.insertar(masClaves[i], "dato_" + to_string(masClaves[i]));
        claves.push_back(masClaves[i]);
    }

    comprobarBusqueda(arbol, claves);
    comprobarOrden(arbol, claves);

    // ------------------------------------------------------------
    // PRUEBA 3: split de un nodo interno y creación de una nueva raíz.
    // Al insertar 90, el separador generado por el split de hoja hace
    // desbordar la raíz interna, por lo que debe dividirse también.
    // ------------------------------------------------------------
    arbol.insertar(90, "dato_90");
    claves.push_back(90);

    comprobarBusqueda(arbol, claves);
    comprobarOrden(arbol, claves);

    // ------------------------------------------------------------
    // PRUEBA 4: inserciones después del split de la raíz.
    // Comprueba que la ruta de búsqueda sigue funcionando en ambos
    // subárboles de la nueva raíz.
    // ------------------------------------------------------------
    const int posteriores[] = {5, 15, 25, 35, 45, 55, 65, 75, 85, 95};
    for (size_t i = 0; i < sizeof(posteriores) / sizeof(posteriores[0]); ++i) {
        arbol.insertar(posteriores[i], "dato_" + to_string(posteriores[i]));
        claves.push_back(posteriores[i]);
    }

    // Ordenamos la lista esperada sin usar std::sort para mantener esta
    // prueba sencilla y explícita.
    for (size_t i = 0; i < claves.size(); ++i) {
        for (size_t j = i + 1; j < claves.size(); ++j) {
            if (claves[j] < claves[i]) {
                int tmp = claves[i];
                claves[i] = claves[j];
                claves[j] = tmp;
            }
        }
    }

    comprobarBusqueda(arbol, claves);
    comprobarOrden(arbol, claves);

    // ------------------------------------------------------------
    // PRUEBA 5: persistencia después de haber creado varios niveles.
    // ------------------------------------------------------------
    arbol.guardarEnArchivo();

    ArbolBPlus recuperado(3, archivo);
    recuperado.cargarDesdeArchivo();

    comprobarBusqueda(recuperado, claves);
    comprobarOrden(recuperado, claves);

    recuperado.eliminarArchivo();

    cout << "OK: todas las pruebas de splits, busqueda, recorrido y persistencia pasaron." << endl;
    return 0;
}
