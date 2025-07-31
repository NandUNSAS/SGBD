#include "../include/funciones.h"
#include <iostream>
#include <fstream>
#include <string>
#include <any>
#include <vector>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <sstream>


using namespace std;

string obtenerRuta(const string& linea) {
    size_t pos = linea.find('#');
    if (pos == string::npos || pos + 1 >= linea.size()) {
        throw runtime_error("Formato de línea inválido.");
    }
    return linea.substr(pos + 1); // ruta
}

string obtenerRutaPorId(const string& archivoRutas, int idBloque) {
    ifstream archivo(archivoRutas);
    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de rutas.");
    }

    string linea;
    int lineaActual = 1;
    while (getline(archivo, linea)) {
        if (lineaActual == idBloque) {
            archivo.close();
            return obtenerRuta(linea); // Extrae el campo después de #
        }
        ++lineaActual;
    }

    archivo.close();
    cout << "ID de bloque fuera de rango en el archivo." << endl;
    return "no encontrado";    
}

size_t obtenerRegistroMasLargo(const string& nombreArchivo, char c) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << "\n";
        return 0;
    }

    string linea;
    if(c == 's' ){
        // Saltar encabezado
        getline(archivo, linea); // línea 1
    }
    

    size_t maxLongitud = 0;

    // Leer desde la segunda línea en adelante
    while (getline(archivo, linea)) {
        size_t longitud = linea.size();  // o .length()
        maxLongitud = max(maxLongitud, longitud);
    }

    archivo.close();
    return maxLongitud;
}

vector<string> dividirLineaCSV(const string& linea) {
    vector<string> campos;
    string campo;
    bool dentroDeComillas = false;

    for (size_t i = 0; i < linea.size(); ++i) {
        char c = linea[i];

        if (c == '"' && (i == 0 || linea[i - 1] != '\\')) {
            dentroDeComillas = !dentroDeComillas;
        } else if (c == ',' && !dentroDeComillas) {
            campos.push_back(campo);
            campo.clear();
        } else {
            campo += c;
        }
    }

    campos.push_back(campo); // último campo
    return campos;
}

// Función para encontrar las longitudes máximas de campos por columna
void encontrarLongitudesMaximas(const string& archivoEntrada, const string& archivoSalida) {
    ifstream entrada(archivoEntrada);
    ofstream salida(archivoSalida, ios::out); // sobreescribe

    if (!entrada || !salida) {
        cerr << "Error al abrir el archivo.\n";
        return;
    }

    string linea;

    // Leer y descartar encabezado
    getline(entrada, linea);

    vector<string> camposMaximos;
    size_t numeroLinea = 0;

    while (getline(entrada, linea)) {
        if (linea.empty()) continue;

        vector<string> campos = dividirLineaCSV(linea);

        if (numeroLinea == 0) {
            camposMaximos = campos;
        } else {
            for (size_t i = 0; i < campos.size(); ++i) {
                if (i >= camposMaximos.size()) {
                    camposMaximos.push_back(campos[i]);
                } else if (campos[i].size() > camposMaximos[i].size()) {
                    camposMaximos[i] = campos[i];
                }
            }
        }

        ++numeroLinea;
    }

    // Escribir en archivo las longitudes máximas separadas por #
    for (size_t i = 0; i < camposMaximos.size(); ++i) {
        salida << camposMaximos[i].size();
        if (i < camposMaximos.size() - 1) salida << "#";
    }

    entrada.close();
    salida.close();

    //cout << "Longitudes máximas por columna escritas en: " << archivoSalida << "\n\n";

}

vector<string> dividir(const string& linea, char delimitador) {
    vector<string> resultado;
    stringstream ss(linea);
    string elemento;

    while (getline(ss, elemento, delimitador)) {
        resultado.push_back(elemento);
    }

    return resultado;
}

string limpiar(const string& texto) {
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    size_t fin = texto.find_last_not_of(" \t\r\n");

    if (inicio == string::npos || fin == string::npos)
        return "";

    return texto.substr(inicio, fin - inicio + 1);
}


// Función para construir y guardar el esquema
void construirEsquema(const string& archivoAtributos,
                      const string& archivoTipos,
                      const string& archivoLongitudes,
                      const string& nombreRelacion,
                      const string& archivoSalida) {
    
    ifstream entradaAtributos(archivoAtributos);
    ifstream entradaTipos(archivoTipos);
    ifstream entradaLongitudes(archivoLongitudes);
    ofstream salida(archivoSalida, ios::out);

    if (!entradaAtributos || !entradaTipos || !entradaLongitudes || !salida) {
        cerr << "Error abriendo archivos de entrada o salida.\n";
        return;
    }

    string lineaAtributos, lineaTipos, lineaLongitudes;
    getline(entradaAtributos, lineaAtributos);
    getline(entradaTipos, lineaTipos);
    getline(entradaLongitudes, lineaLongitudes);

    vector<string> atributos = dividir(lineaAtributos, ',');
    vector<string> tipos = dividir(lineaTipos, '#');
    vector<string> longitudes = dividir(lineaLongitudes, '#');

        // Limpiar atributos
    for (string& atributo : atributos)
        atributo = limpiar(atributo);

    // Limpiar tipos
    for (string& tipo : tipos)
        tipo = limpiar(tipo);

    // Limpiar longitudes
    for (string& longitud : longitudes)
        longitud = limpiar(longitud);

    if (atributos.size() != tipos.size() || atributos.size() != longitudes.size()) {
        cerr << "Cantidad inconsistente de atributos, tipos o longitudes.\n";
        return;
    }

    salida << nombreRelacion;

    for (size_t i = 0; i < atributos.size(); ++i) {
        salida << "#" << atributos[i]
               << "#" << tipos[i]
               << "#" << longitudes[i];
    }

    cout << "Esquema guardado correctamente en: " << archivoSalida << endl;
}






//