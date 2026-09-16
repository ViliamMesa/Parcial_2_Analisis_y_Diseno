/**
 * Archivo: AnalizadorSQL.h
 * Propósito: Define la clase encargada de tomar cadenas de texto, identificar  
 *            los comandos (INSERT, SELECT, CREATE) y delegar la carga de trabajo
 *            al motor de almacenamiento subyacente (Arbol B+).
 */
#ifndef ANALIZADOR_SQL_H
#define ANALIZADOR_SQL_H

#include <iostream>  // Salida estandar a consola
#include <string>    // Operaciones con cadenas
#include <sstream>   // 'stringstream' permite partir un texto en palabras facilmente
#include <vector>    // Uso de arrays dinamicos
#include "ArbolBPlus.h" // Se requiere para poder despachar los comandos al arbol

using namespace std;

class AnalizadorSQL {
private:
    ArbolBPlus* bd; // Puntero a la base de datos instanciada en el main

    // Convierte cualquier cadena a mayúsculas para evitar problemas de "Select" vs "SELECT"
    string aMayusculas(string cadena);

public:
    // Constructor que recibe el árbol B+ y lo guarda como atributo
    AnalizadorSQL(ArbolBPlus* base_datos);

    /**
     * @brief Método de entrada del analizador. Recibe la consulta en crudo.
     * Separa la primera palabra de la consulta para derivar el flujo.
     */
    void ejecutarConsulta(string consulta);

    // ==========================================
    // PARSEO DE DDL (Lenguaje de Definición de Datos)
    // ==========================================
    // Comandos enfocados en la estructura global (Ej. CREATE TABLE, DROP TABLE)
    void analizarDDL(string consulta, string comando);

    // ==========================================
    // PARSEO DE DQL y DML (Consultas y Manipulación de Datos)
    // ==========================================
    // Comandos enfocados en la información interna (Ej. INSERT, SELECT, DELETE)
    void analizarDQL_DML(string consulta, string comando);

    // Muestra en consola las ayudas formatadas con colores ANSI
    void mostrarAyuda();
};

#endif // ANALIZADOR_SQL_H
