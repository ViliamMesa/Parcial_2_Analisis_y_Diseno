/**
 * Archivo: main.cpp
 * Propósito: Punto de entrada de la aplicación. Gestiona la interfaz de línea 
 *            de comandos (CLI) y el bucle interactivo (REPL) para que el usuario 
 *            ingrese comandos SQL.
 */
#include <iostream>      // Permite entrada/salida por consola (cin, cout, getline)
#include <string>        // Manejo del tipo de dato 'string' para textos y consultas
#include "ArbolBPlus.h"  // Archivo local: Contiene el motor de almacenamiento de la base de datos
#include "AnalizadorSQL.h" // Archivo local: Contiene el procesador de comandos SQL

using namespace std;

int main(int argc, char* argv[]) {
    // Manejo de argumentos pasados por la terminal al ejecutar el binario
    // Sirve para mostrar la ayuda rápidamente sin entrar al bucle interactivo.
    if (argc > 1) {
        string argumento = argv[1];
        if (argumento == "--help" || argumento == "-h") {
            cout << "Uso de la aplicacion por CLI:\n";
            cout << "  ./bd_sql          : Inicia la consola interactiva SQL.\n";
            cout << "  ./bd_sql --help   : Muestra esta ayuda.\n";
            return 0; // Termina la ejecución tras mostrar la ayuda
        }
    }

    cout << "Inicializando Base de Datos con Arbol B+ (Grado 3)...\n";
    
    // 1. Instanciar el gestor del Árbol B+
    // El grado define la cantidad máxima de hijos/claves por nodo.
    // "base_datos.txt" es el archivo donde se guardará la información de forma persistente.
    ArbolBPlus arbolBD(3, "base_datos.txt");
    
    // 2. Intentar cargar los datos existentes en disco (si el archivo ya existe)
    arbolBD.cargarDesdeArchivo();

    // 3. Instanciar el Analizador SQL (Parser)
    // Se le pasa la referencia del árbol para que el analizador pueda ejecutar
    // operaciones sobre él cuando detecte comandos válidos (como INSERT o SELECT).
    AnalizadorSQL analizador(&arbolBD);

    string consulta;
    // Mensaje de bienvenida para el estudiante
    cout << "\n[Consola SQL Arbol B+ - PARCIAL 2]\n";
    cout << "Escriba 'HELP' para ver los comandos, 'EXIT' para salir.\n";

    // Bucle REPL (Read-Eval-Print Loop)
    // Este bucle mantiene el programa vivo, pidiendo texto infinitamente hasta que el usuario decida salir.
    while (true) {
        // Indicador visual (prompt)
        cout << "sql> ";
        getline(cin, consulta);

        // Si el usuario da "Enter" sin escribir nada, ignorar y volver a preguntar
        if (consulta.empty()) continue;

        // Comprobación de la orden de salida
        if (consulta == "EXIT" || consulta == "exit" || consulta == "quit") {
            cout << "Guardando cambios y saliendo...\n";
            // Guardar los datos en el archivo de texto antes de cerrar para no perder la información
            arbolBD.guardarEnArchivo();
            break; // Romper el ciclo y terminar el programa
        }

        // Si no es "EXIT", delegar la responsabilidad de entender la cadena de texto al analizador
        analizador.ejecutarConsulta(consulta);
    }

    return 0;
}
