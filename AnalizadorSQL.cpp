/**
 * Archivo: AnalizadorSQL.cpp
 * Propósito: Analiza y ejecuta el subconjunto SQL definido para el parcial.
 */
#include "AnalizadorSQL.h"
#include <algorithm>
#include <cctype>

AnalizadorSQL::AnalizadorSQL(ArbolBPlus* base_datos) : bd(base_datos) {}

AnalizadorSQL::~AnalizadorSQL() {
    destruirIndices();
}

string AnalizadorSQL::aMayusculas(string cadena) {
    for (size_t i = 0; i < cadena.size(); ++i)
        cadena[i] = static_cast<char>(toupper(static_cast<unsigned char>(cadena[i])));
    return cadena;
}

string AnalizadorSQL::quitarEspacios(string cadena) {
    size_t inicio = cadena.find_first_not_of(" \t\n\r");
    if (inicio == string::npos) return "";
    size_t fin = cadena.find_last_not_of(" \t\n\r");
    return cadena.substr(inicio, fin - inicio + 1);
}

string AnalizadorSQL::quitarPuntoYComa(string cadena) {
    cadena = quitarEspacios(cadena);
    while (!cadena.empty() && cadena[cadena.size() - 1] == ';') {
        cadena.erase(cadena.size() - 1);
        cadena = quitarEspacios(cadena);
    }
    return cadena;
}

vector<string> AnalizadorSQL::separarPorComas(string texto) {
    vector<string> partes;
    string actual;
    bool dentroComillas = false;

    for (size_t i = 0; i < texto.size(); ++i) {
        char c = texto[i];
        if (c == '\'') dentroComillas = !dentroComillas;

        if (c == ',' && !dentroComillas) {
            partes.push_back(quitarEspacios(actual));
            actual.clear();
        } else {
            actual += c;
        }
    }

    if (!actual.empty()) partes.push_back(quitarEspacios(actual));
    return partes;
}

int AnalizadorSQL::posicionColumna(string columna) {
    columna = aMayusculas(quitarEspacios(columna));
    for (size_t i = 0; i < columnas.size(); ++i) {
        if (aMayusculas(columnas[i]) == columna)
            return static_cast<int>(i);
    }
    return -1;
}

string AnalizadorSQL::extraerCampo(string datos, int posicion) {
    vector<string> campos = separarPorComas(datos);
    if (posicion < 0 || posicion >= static_cast<int>(campos.size())) return "";

    string campo = quitarEspacios(campos[posicion]);
    if (campo.size() >= 2 && campo.front() == '\'' && campo.back() == '\'')
        campo = campo.substr(1, campo.size() - 2);
    return campo;
}

bool AnalizadorSQL::convertirEntero(string texto, int& valor) {
    texto = quitarEspacios(texto);
    if (texto.empty()) return false;

    size_t inicio = (texto[0] == '-' || texto[0] == '+') ? 1 : 0;
    if (inicio == texto.size()) return false;

    for (size_t i = inicio; i < texto.size(); ++i) {
        if (!isdigit(static_cast<unsigned char>(texto[i]))) return false;
    }

    try {
        valor = stoi(texto);
        return true;
    } catch (...) {
        return false;
    }
}

void AnalizadorSQL::destruirIndices() {
    for (size_t i = 0; i < indices.size(); ++i)
        delete indices[i].arbol;
    indices.clear();
}

void AnalizadorSQL::reconstruirIndices() {
    vector<Registro> registros = bd->obtenerTodos();

    for (size_t i = 0; i < indices.size(); ++i) {
        IndiceSecundario& indice = indices[i];
        indice.arbol->limpiar();

        for (size_t j = 0; j < registros.size(); ++j) {
            int claveIndice;

            // La columna 0 es la clave primaria.
            if (indice.posicionColumna == 0) {
                claveIndice = registros[j].clave;
            } else {
                // Las columnas adicionales están serializadas en datos.
                string valor = extraerCampo(registros[j].datos,
                                            indice.posicionColumna - 1);
                if (!convertirEntero(valor, claveIndice))
                    continue;
            }

            // El segundo árbol almacena como valor los IDs de las filas
            // originales. Así un mismo valor de índice puede apuntar a varias filas.
            string ids = indice.arbol->buscar(claveIndice);
            if (!ids.empty()) ids += ",";
            ids += to_string(registros[j].clave);
            indice.arbol->insertar(claveIndice, ids);
        }
    }
}

void AnalizadorSQL::ejecutarConsulta(string consulta) {
    consulta = quitarEspacios(consulta);
    if (consulta.empty()) return;

    stringstream ss(consulta);
    string comando;
    ss >> comando;
    comando = aMayusculas(comando);

    if (comando == "CREATE" || comando == "DROP") {
        analizarDDL(consulta, comando);
    } else if (comando == "SELECT" || comando == "INSERT" || comando == "DELETE") {
        analizarDQL_DML(consulta, comando);
    } else if (comando == "HELP") {
        mostrarAyuda();
    } else {
        cout << "Error: Comando SQL no reconocido. Escriba HELP para mas informacion.\n";
    }
}

void AnalizadorSQL::analizarDDL(string consulta, string comando) {
    string limpia = quitarPuntoYComa(consulta);
    string mayus = aMayusculas(limpia);

    if (comando == "CREATE") {
        if (mayus.find("CREATE TABLE") == 0) {
            size_t inicioNombre = string("CREATE TABLE").size();
            size_t paren = limpia.find('(', inicioNombre);
            if (paren == string::npos) {
                cout << "Error: CREATE TABLE requiere las columnas entre parentesis.\n";
                return;
            }

            nombreTabla = quitarEspacios(limpia.substr(inicioNombre, paren - inicioNombre));
            if (nombreTabla.empty()) {
                cout << "Error: debe indicar el nombre de la tabla.\n";
                return;
            }

            size_t cierre = limpia.rfind(')');
            if (cierre == string::npos || cierre <= paren) {
                cout << "Error: definicion de columnas invalida.\n";
                return;
            }

            columnas.clear();
            vector<string> definiciones = separarPorComas(
                limpia.substr(paren + 1, cierre - paren - 1));

            for (size_t i = 0; i < definiciones.size(); ++i) {
                stringstream col(definiciones[i]);
                string nombreColumna;
                col >> nombreColumna;
                if (!nombreColumna.empty())
                    columnas.push_back(nombreColumna);
            }

            cout << "Tabla '" << nombreTabla << "' creada. Arbol B+ primario listo.\n";
        }
        else if (mayus.find("CREATE INDEX") == 0) {
            size_t inicio = string("CREATE INDEX").size();
            size_t onPos = mayus.find(" ON ", inicio);
            if (onPos == string::npos) {
                cout << "Error: sintaxis CREATE INDEX invalida.\n";
                return;
            }

            string nombreIndice = quitarEspacios(limpia.substr(inicio, onPos - inicio));
            size_t inicioTabla = onPos + 4;
            size_t parentesis = limpia.find('(', inicioTabla);
            if (parentesis == string::npos) {
                cout << "Error: falta la columna del indice.\n";
                return;
            }

            string tabla = quitarEspacios(limpia.substr(
                inicioTabla, parentesis - inicioTabla));
            size_t cierre = limpia.find(')', parentesis);
            if (cierre == string::npos) {
                cout << "Error: falta cerrar la columna del indice.\n";
                return;
            }

            string columna = quitarEspacios(limpia.substr(
                parentesis + 1, cierre - parentesis - 1));

            if (nombreTabla.empty() ||
                aMayusculas(tabla) != aMayusculas(nombreTabla)) {
                cout << "Error: la tabla indicada no coincide con la tabla creada.\n";
                return;
            }

            int posicion = posicionColumna(columna);
            if (posicion < 0) {
                cout << "Error: columna no encontrada: " << columna << "\n";
                return;
            }

            for (size_t i = 0; i < indices.size(); ++i) {
                if (aMayusculas(indices[i].nombre) == aMayusculas(nombreIndice)) {
                    cout << "Error: el indice ya existe.\n";
                    return;
                }
            }

            // El esqueleto define claves B+ como enteros. El índice secundario
            // conserva esa misma estructura y almacena los IDs primarios como valor.
            ArbolBPlus* arbolIndice = new ArbolBPlus(3, "");
            indices.push_back(IndiceSecundario(nombreIndice, columna,
                                                posicion, arbolIndice));
            reconstruirIndices();

            cout << "Indice '" << nombreIndice << "' creado sobre la columna '"
                 << columna << "'.\n";
            cout << "Nota: el valor de la columna indexada debe ser numerico en "
                 << "este esqueleto.\n";
        }
        else {
            cout << "Error: comando CREATE no reconocido. Use CREATE TABLE o CREATE INDEX.\n";
        }
    }
    else if (comando == "DROP") {
        string prefijo = "DROP TABLE";
        if (mayus.find(prefijo) != 0) {
            cout << "Error: solo se admite DROP TABLE.\n";
            return;
        }

        string tabla = quitarEspacios(limpia.substr(prefijo.size()));
        if (!nombreTabla.empty() &&
            aMayusculas(tabla) != aMayusculas(nombreTabla)) {
            cout << "Error: la tabla indicada no coincide con la tabla creada.\n";
            return;
        }

        destruirIndices();
        bd->limpiar();
        bd->eliminarArchivo();
        nombreTabla.clear();
        columnas.clear();

        cout << "Tabla eliminada y archivo de persistencia destruido.\n";
    }
}

void AnalizadorSQL::analizarDQL_DML(string consulta, string comando) {
    string limpia = quitarPuntoYComa(consulta);
    string mayus = aMayusculas(limpia);

    if (nombreTabla.empty()) {
        cout << "Error: primero debe crear una tabla con CREATE TABLE.\n";
        return;
    }

    if (comando == "INSERT") {
        string prefijo = "INSERT INTO";
        size_t inicio = mayus.find(prefijo);
        size_t valuesPos = mayus.find(" VALUES ", inicio + prefijo.size());

        if (inicio != 0 || valuesPos == string::npos) {
            cout << "Error: sintaxis INSERT invalida.\n";
            return;
        }

        string tabla = quitarEspacios(limpia.substr(
            prefijo.size(), valuesPos - prefijo.size()));
        if (aMayusculas(tabla) != aMayusculas(nombreTabla)) {
            cout << "Error: tabla no encontrada.\n";
            return;
        }

        size_t abre = limpia.find('(', valuesPos);
        size_t cierre = limpia.rfind(')');
        if (abre == string::npos || cierre == string::npos || cierre <= abre) {
            cout << "Error: faltan los valores del INSERT.\n";
            return;
        }

        vector<string> valores = separarPorComas(
            limpia.substr(abre + 1, cierre - abre - 1));

        if (valores.size() < 1) {
            cout << "Error: INSERT sin valores.\n";
            return;
        }

        int id;
        if (!convertirEntero(valores[0], id)) {
            cout << "Error: la clave primaria debe ser un entero.\n";
            return;
        }

        string datos;
        for (size_t i = 1; i < valores.size(); ++i) {
            if (i > 1) datos += ",";
            datos += quitarEspacios(valores[i]);
        }

        bd->insertar(id, datos);
        reconstruirIndices();
        bd->guardarEnArchivo();
        cout << "Registro insertado correctamente.\n";
    }
    else if (comando == "SELECT") {
        if (mayus.find("SELECT * FROM") != 0) {
            cout << "Error: solo se admite SELECT *.\n";
            return;
        }

        size_t inicioTabla = string("SELECT * FROM").size();
        size_t wherePos = mayus.find(" WHERE ", inicioTabla);
        string tabla = quitarEspacios(limpia.substr(
            inicioTabla,
            wherePos == string::npos ? string::npos : wherePos - inicioTabla));

        if (aMayusculas(tabla) != aMayusculas(nombreTabla)) {
            cout << "Error: tabla no encontrada.\n";
            return;
        }

        if (wherePos != string::npos) {
            string condicion = quitarEspacios(limpia.substr(wherePos + 7));
            string condicionMayus = aMayusculas(condicion);
            size_t igual = condicion.find('=');

            if (condicionMayus.find("ID") != 0 || igual == string::npos) {
                cout << "Error: la unica condicion soportada es WHERE id = <id>.\n";
                return;
            }

            int id;
            if (!convertirEntero(condicion.substr(igual + 1), id)) {
                cout << "Error: ID invalido.\n";
                return;
            }

            string resultado = bd->buscar(id);
            if (resultado.empty()) {
                cout << "Registro no encontrado.\n";
            } else {
                cout << id << " | " << resultado << "\n";
            }
        }
        else {
            vector<Registro> registros = bd->obtenerTodos();
            if (registros.empty()) {
                cout << "No hay registros.\n";
                return;
            }

            for (size_t i = 0; i < registros.size(); ++i)
                cout << registros[i].clave << " | " << registros[i].datos << "\n";
        }
    }
    else if (comando == "DELETE") {
        string prefijo = "DELETE FROM";
        if (mayus.find(prefijo) != 0) {
            cout << "Error: sintaxis DELETE invalida.\n";
            return;
        }

        size_t wherePos = mayus.find(" WHERE ", prefijo.size());
        if (wherePos == string::npos) {
            cout << "Error: DELETE requiere WHERE id = <id>.\n";
            return;
        }

        string tabla = quitarEspacios(limpia.substr(
            prefijo.size(), wherePos - prefijo.size()));
        if (aMayusculas(tabla) != aMayusculas(nombreTabla)) {
            cout << "Error: tabla no encontrada.\n";
            return;
        }

        string condicion = quitarEspacios(limpia.substr(wherePos + 7));
        size_t igual = condicion.find('=');
        if (igual == string::npos ||
            aMayusculas(quitarEspacios(condicion.substr(0, igual))) != "ID") {
            cout << "Error: la unica condicion soportada es WHERE id = <id>.\n";
            return;
        }

        int id;
        if (!convertirEntero(condicion.substr(igual + 1), id)) {
            cout << "Error: ID invalido.\n";
            return;
        }

        bd->eliminar(id);
        reconstruirIndices();
        bd->guardarEnArchivo();
        cout << "DELETE ejecutado.\n";
    }
}

void AnalizadorSQL::mostrarAyuda() {
    cout << "\n=== Motor SQL con Arboles B+ ===\n";
    cout << "CREATE TABLE usuarios (id INT, edad INT, nombre STR)\n";
    cout << "CREATE INDEX idx_edad ON usuarios (edad)\n";
    cout << "DROP TABLE usuarios\n";
    cout << "INSERT INTO usuarios VALUES (10, 20, 'Juan')\n";
    cout << "SELECT * FROM usuarios\n";
    cout << "SELECT * FROM usuarios WHERE id = 10\n";
    cout << "DELETE FROM usuarios WHERE id = 10\n";
    cout << "HELP\n";
    cout << "EXIT\n\n";
}
