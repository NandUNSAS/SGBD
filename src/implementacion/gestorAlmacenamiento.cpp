#include "../include/gestorAlmacenamiento.h"
#include "../include/disco.h"
#include <iostream>
#include <fstream>
#include <string>
#include "../include/funcionesBloque.h"
#include "../include/controladorDisco.h"
#include "insertar.h"
//
using namespace std;

void gestorAlmacenamiento::setDisco(const disco& _discoDuro){
    discoDuro = _discoDuro;
}

void gestorAlmacenamiento::calcularCapacidadBloque(){
    capacidadBloque = discoDuro.getTamSector()*sectoresXbloque;
}

void gestorAlmacenamiento::setSectoresXbloque(int _sectoresXbloque){
    sectoresXbloque = _sectoresXbloque;
}

void gestorAlmacenamiento::setLongitudRegistroMax(int _longitudRegistroMax){
    longitudRegistroMax = _longitudRegistroMax;
}

int gestorAlmacenamiento::getCapacidadBloque(){
    return capacidadBloque;
}

int gestorAlmacenamiento::getBloquesXpista(){
    return bloquesXpista;
}

int gestorAlmacenamiento::getBloquesXplato(){
    return bloquesXplato;
}

int gestorAlmacenamiento::getBloquesTotales(){
    return bloquesTotales;
}

void gestorAlmacenamiento::calcularBloquesXpista(){
    bloquesXpista = discoDuro.getSectoresXpista()/sectoresXbloque;
}

void gestorAlmacenamiento::calcularBloquesXplato(){
    bloquesXplato = 2*(discoDuro.getPistasXsuperficie()*discoDuro.getSectoresXpista())/sectoresXbloque;
}

void gestorAlmacenamiento::calcularBloquesTotales(){
   bloquesTotales = (discoDuro.getPlatos()*2*discoDuro.getPistasXsuperficie()*discoDuro.getSectoresXpista())/sectoresXbloque;
}


void gestorAlmacenamiento::establecerHeaders(const string& rutaSector, string rutaPlantillaHeader) {
    // Leer la primera línea de la plantilla
    ifstream archivoHeader(rutaPlantillaHeader);
    if (!archivoHeader.is_open()) {
        cerr << "Error al abrir el archivo de plantilla: " << rutaPlantillaHeader << endl;
        return;
    }

    string lineaHeader;
    getline(archivoHeader, lineaHeader); // Leer solo la primera línea
    archivoHeader.close();

    // Escribir la línea del header en el archivo sector
    ofstream archivoSector(rutaSector, ios::trunc); // trunc para sobrescribir si ya existe
    if (!archivoSector.is_open()) {
        cerr << "Error al abrir el archivo de sector: " << rutaSector << endl;
        return;
    }

    archivoSector << lineaHeader << '\n'; // Escribir header y salto de línea
    archivoSector.close();
}

void gestorAlmacenamiento::generarArchivoRutas(){
    cout << "Crear archivo de rutas en orden por pista-sector...\n";
    string rutaBase = "../../DiscoLocal";
    string rutaArchivo = "../../rutas_sectores/rutas.txt";
    string rutaBloques = "../../rutas_sectores/cilindroMedio.txt";

    ofstream rutasOut(rutaArchivo);
    if (!rutasOut.is_open()) {
        cerr << "Error al crear rutas.txt\n";
        return;
    }

    ofstream _rutasBloques(rutaBloques);
    if (!_rutasBloques.is_open()) {
        cerr << "Error al crear cilindroMedio.txt\n";
        return;
    }

    int platos = discoDuro.getPlatos();
    int superficies = 2; // fijo
    int pistas = discoDuro.getPistasXsuperficie();
    int sectores = discoDuro.getSectoresXpista();
    int mitadDisco = pistas / 2;

    // Orden: pista -> sector -> plato -> superficie
    for (int m = 1; m <= pistas; m++) { // pista
        for (int n = 1; n <= sectores; n++) { // sector
            for (int p = 1; p <= platos; p++) {
                for (int s = 1; s <= superficies; s++) {
                    string rutaSector = rutaBase +
                        "/plato" + to_string(p) +
                        "/superficie" + to_string(s) +
                        "/pista" + to_string(m) +
                        "/sector" + to_string(n) + ".txt";

                    if (m == mitadDisco) {
                        _rutasBloques << "0#" << rutaSector << '\n';
                        establecerHeaders(rutaSector, "../../archivos_estructura_bloque/head_bloque_fijo.txt");
                        int capacidad = discoDuro.getCapacidadDisco() - 17;
                        discoDuro.setCapacidadDisco(capacidad);
                    } else {
                        rutasOut << "0#" << rutaSector << '\n';
                        establecerHeaders(rutaSector, "../../archivos_estructura_bloque/head_bloque_fijo.txt");
                        int capacidad = discoDuro.getCapacidadDisco() - 17;
                        discoDuro.setCapacidadDisco(capacidad);
                    }
                }
            }
        }
    }

    rutasOut.close();
    _rutasBloques.close();
    cout << "Archivo rutas.txt generado correctamente.\n";
}

void gestorAlmacenamiento::insertarRutasBloques(const string& rutaReservadoBloques, const string& rutaSectores) {
    ifstream archivoBloques(rutaReservadoBloques);
    if (!archivoBloques.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de bloques: " << rutaReservadoBloques << "\n";
        return;
    }

    ifstream archivoSectores(rutaSectores);
    if (!archivoSectores.is_open()) {
        cerr << "Error: No se pudo abrir el archivo de sectores: " << rutaSectores << "\n";
        archivoBloques.close();  // buena práctica
        return;
    }

    string basura, rutaBloque, rutaSector;

    // Leer bloques uno por uno
    while (getline(archivoBloques, basura, '#') && getline(archivoBloques, rutaBloque)) {
        int contador = 0;

        while (contador < sectoresXbloque && getline(archivoSectores, rutaSector)) {
            insertarRegistroB(rutaSector, rutaBloque, longitudRegistroMax);

            int capacidad = discoDuro.getCapacidadDisco() - longitudRegistroMax;
            discoDuro.setCapacidadDisco(capacidad);

            contador++;
        }

        if (archivoSectores.eof()) break;
    }

    archivoBloques.close();
    archivoSectores.close();
}


/*
void gestorAlmacenamiento::definirBloques(){
    
}
*/

void gestorAlmacenamiento::insertarRegistro(string registro, string rutaSector, int tamanoRegistro){
    
    string ruta_sector_disponible = rutaSector;
    bool insercion_exitosa = actualizarCabecera(tamanoRegistro, rutaSector, discoDuro.getTamSector());
    if(!insercion_exitosa){
        ruta_sector_disponible = bloqueDisponible("../../rutas_sectores/cilindroMedio.txt");
        insercion_exitosa = actualizarCabecera(tamanoRegistro, ruta_sector_disponible, discoDuro.getTamSector());
    
    }
    cd.insertar(registro, tamanoRegistro, ruta_sector_disponible);
    int capacidad = discoDuro.getCapacidadDisco() - tamanoRegistro;
    discoDuro.setCapacidadDisco(capacidad);
    verificadorSectoresLlenos("../../rutas_sectores/cilindroMedio.txt");
    verificadorBloquesLlenos("../../rutas_sectores/cilindroMedio.txt");
}


void gestorAlmacenamiento::insertarRegistroB(string registro, string rutaSector, int tamanoRegistro){
    string ruta_sector_disponible = rutaSector;
    bool insercion_exitosa = actualizarCabeceraB(tamanoRegistro, rutaSector, discoDuro.getTamSector(), sectoresXbloque);
    if(!insercion_exitosa){
        ruta_sector_disponible = bloqueDisponible("../../rutas_sectores/cilindroMedio.txt");
        insercion_exitosa = actualizarCabeceraB(tamanoRegistro, ruta_sector_disponible, discoDuro.getTamSector(), sectoresXbloque);
    
    }
    cd.insertar(registro, tamanoRegistro, ruta_sector_disponible);
    
}

void gestorAlmacenamiento::guardarEnArchivo(){
    ofstream archivo("../../archivo_info_Disco/info_bloque.txt");
    if (archivo.is_open()){
        archivo << sectoresXbloque;
        archivo.close();
    }
    else {
        cerr << "No se pudo abrir el archivo info_bloque.txt";
    }

    discoDuro.guardarEnArchivo();
}

int gestorAlmacenamiento::capacidadDisco(){
    int valor = discoDuro.getCapacidadDisco();
    return valor;
}