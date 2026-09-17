/**
 * Archivo: AnalizadorSQL.h
 * Propósito: Parser sencillo para el subconjunto SQL solicitado en el parcial.
 */
#ifndef ANALIZADOR_SQL_H
#define ANALIZADOR_SQL_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "ArbolBPlus.h"

using namespace std;

class AnalizadorSQL {
private:
    struct IndiceSecundario {
        string nombre;
        string columna;
        int posicionColumna;
        ArbolBPlus* arbol;

        IndiceSecundario(string n, string c, int p, ArbolBPlus* a)
            : nombre(n), columna(c), posicionColumna(p), arbol(a) {}
    };

    ArbolBPlus* bd;
    string nombreTabla;
    vector<string> columnas;
    vector<IndiceSecundario> indices;

    string aMayusculas(string cadena);
    string quitarEspacios(string cadena);
    string quitarPuntoYComa(string cadena);
    vector<string> separarPorComas(string texto);
    int posicionColumna(string columna);
    string extraerCampo(string datos, int posicion);
    bool convertirEntero(string texto, int& valor);
    void reconstruirIndices();
    void destruirIndices();

public:
    AnalizadorSQL(ArbolBPlus* base_datos);
    ~AnalizadorSQL();

    void ejecutarConsulta(string consulta);
    void analizarDDL(string consulta, string comando);
    void analizarDQL_DML(string consulta, string comando);
    void mostrarAyuda();
};

#endif // ANALIZADOR_SQL_H
