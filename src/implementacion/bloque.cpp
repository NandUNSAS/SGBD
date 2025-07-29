#include "../include/bloque.h"
#include <string>
#include "../include/funciones.h"
#include "../include/funcionesBloque.h"
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// Constructor vacío
bloque::bloque() {
    // Puedes inicializar valores por defecto si quieres
    idBloque = 0;
    rutaBloque = "";
    contenido = "";
}

// Set para idBloque
void bloque::setIdBloque(int id) {
    idBloque = id;
}

// Get para idBloque
int bloque::getIdBloque() const {
    return idBloque;
}

// Set para rutaBloque
void bloque::setRutaBloque(const string& ruta) {
    rutaBloque = ruta;
}

// Get para rutaBloque
string bloque::getRutaBloque() const {
    return rutaBloque;
}

string bloque::getContenido() const {
    return contenido;
}
void bloque::construirBloque() {
    contenido = ""; // Inicializa como vacío

    ifstream archivoBloque(rutaBloque);
    if (!archivoBloque.is_open()) {
        cerr << "Error al abrir el archivo de bloque: " << rutaBloque << endl;
        return;
    }

    string lineaRuta;
    getline(archivoBloque, lineaRuta); // Primera línea del archivo bloque
    
    while (getline(archivoBloque, lineaRuta)) {
        vector<string> campos = obtenerCampos(lineaRuta);
        if (campos.size() < 2) continue;

        string estado = campos[0];
        string rutaSector = campos[1];
        cout << "Procesando sector: " << rutaSector << endl;

        // Procesar archivo del sector
        ifstream archivoSector(rutaSector);
        if (!archivoSector.is_open()) {
            cerr << "No se pudo abrir el archivo del sector: " << rutaSector << endl;
            continue;
        }

        // Leer y guardar la cabecera del sector
        string cabeceraSector;
        if (!getline(archivoSector, cabeceraSector)) {
            cerr << "Error leyendo cabecera del sector" << endl;
            archivoSector.close();
            continue;
        }

        // Añadir la cabecera al contenido del bloque
        contenido += cabeceraSector + "\n";

        vector<string> datosCabecera = obtenerCampos(cabeceraSector);
        if (datosCabecera.size() < 5) {
            cerr << "Cabecera de sector inválida" << endl;
            archivoSector.close();
            continue;
        }

        int registrosEsperados = stoi(datosCabecera[4]);
        int registrosLeidos = 0;

        // Leer registros del sector
        string lineaSector;
        while (getline(archivoSector, lineaSector)) {
            if (!lineaSector.empty()) {
                contenido += lineaSector + "\n";
                registrosLeidos++;
            }
        }

        // Completar con líneas vacías si faltan registros
        for (int i = registrosLeidos; i < registrosEsperados; i++) {
            contenido += "\n"; // Línea vacía para registro faltante
        }

        archivoSector.close();
    }

    archivoBloque.close();
}


void bloque::mostrarBloque() const {
    cout << "ID: " << idBloque << endl;
    cout << "Ruta del bloque: " << rutaBloque << endl;
    cout << "Registros: " << contenido << endl;
}

void bloque::inicializarBloque(int id, string ruta){
    idBloque = id;
    rutaBloque = ruta;
    construirBloque();
}



void bloque::leerRutasSectoresDesdeArchivo() {
    rutaSectores.clear();

    ifstream archivo(rutaBloque);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << rutaBloque << endl;
        return;
    }

    string linea;
    bool primeraLinea = true;

    while (getline(archivo, linea)) {
        if (primeraLinea) {
            primeraLinea = false;
            continue; // Saltar encabezado
        }

        if (linea.empty()) continue;

        size_t pos = linea.find('#');
        if (pos != string::npos) {
            string estadoStr = linea.substr(0, pos);
            string ruta = linea.substr(pos + 1);

            EntradaSector entrada;
            entrada.estado = stoi(estadoStr);
            entrada.rutaSector = ruta;

            rutaSectores.push_back(entrada);
        }
    }

    archivo.close();
}
