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
    getline(archivoBloque,lineaRuta);
    while (getline(archivoBloque, lineaRuta)) {
        vector<string> campos = obtenerCampos(lineaRuta);
        if (campos.size() < 2) continue;

        string estado = campos[0];
        string rutaSector = campos[1];
        cout << "ruta sector del bloque "<< rutaSector << endl;

        ifstream archivoSector(rutaSector);
        if (!archivoSector.is_open()) {
            cerr << "No se pudo abrir el archivo del sector: " << rutaSector << endl;
            continue;
        }

        string lineaSector;
        while (getline(archivoSector, lineaSector)) {
            contenido += lineaSector + "\n"; // Añade línea al bloque
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