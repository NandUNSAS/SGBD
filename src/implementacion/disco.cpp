#include "../include/disco.h"
#include "iostream"
using namespace std;
#include <filesystem>
#include <string>
#include <fstream>
namespace fs = filesystem;
//
disco::disco(){
    platos = 0;
    superficies = 2;
    pistasXsuperficie = 0;
    sectoresXpista = 0;
    tamSector = 0;
    capacidadDisco = 0;
}

void disco::discoPersonalizado(int _platos, int _pistasXsuperficie, int _sectorXpista, int _tamSector) {
    platos = _platos;
    superficies = 2;
    pistasXsuperficie = _pistasXsuperficie;
    sectoresXpista = _sectorXpista;
    tamSector = _tamSector; 
    
}    

void disco::discoEstatico() {
    platos = 8;
    superficies = 2;
    pistasXsuperficie = 10;
    sectoresXpista = 15;
    tamSector = 1024;
}


/*
void disco::crearDisco(){
    
    string ruta = "/home/asus/Documentos/BD_1/DiscoLocal";
    if (fs::create_directory(ruta)) {
        cout << "Carpeta creada exitosamente.\n";
    } else {
        cout << "No se pudo crear la carpeta (quizá ya existe).\n";
    }

    for (int i = 1; i <= platos*superficies; i++) {
        fs::create_directory(ruta + "/plato" + to_string(i));
        for (int m = 1; m <= pistasXplato; m++){
            fs::create_directory(ruta + "/plato" + to_string(i) + "/pista" + to_string(m));
            for (int n = 1; n <= sectoresXpista; n++){
                ofstream archivo(ruta + "/plato" + to_string(i) + "/pista" + to_string(m) + "/sector" + to_string(n) + ".txt");
                archivo.close();
            }
        }
    }
 }
/
*/
void disco::crearDisco() {
    string ruta = "../../DiscoLocal";

    if (fs::create_directory(ruta)) {
        cout << "Carpeta creada exitosamente.\n";
    } else {
        cout << "No se pudo crear la carpeta (quizá ya existe).\n";
    }

    for (int p = 1; p <= platos; p++) {
        string rutaPlato = ruta + "/plato" + to_string(p);
        fs::create_directory(rutaPlato);

        for (int s = 1; s <= superficies; s++) {
            string rutaSuperficie = rutaPlato + "/superficie" + to_string(s);
            fs::create_directory(rutaSuperficie);

            for (int m = 1; m <= pistasXsuperficie; m++) {
                string rutaPista = rutaSuperficie + "/pista" + to_string(m);
                fs::create_directory(rutaPista);

                for (int n = 1; n <= sectoresXpista; n++) {
                    string rutaSector = rutaPista + "/sector" + to_string(n) + ".txt";
                    ofstream archivo(rutaSector);
                    
                    archivo.close();
                }
            }
        }
    }
}

void disco::guardarEnArchivo() {
ofstream archivo("archivo_info_Disco/info_disco.txt");
    if (archivo.is_open()) {
        archivo << getPlatos() << "#"
                << getSuperficie() << "#"
                << getPistasXsuperficie() << "#"
                << getSectoresXpista() << "#"
                << getTamSector() << "#"
                << getCapacidadDisco();
        archivo.close();
    } else {
        cerr << "No se pudo abrir el archivo datos_disco.txt para escritura." << std::endl;
    }
}



void disco::calcularCapacidadDisco(){
    capacidadDisco = platos*superficies*pistasXsuperficie*sectoresXpista*tamSector;
 }

void disco::setCapacidadDisco( int _capacidadDisco){
    capacidadDisco = _capacidadDisco;
}

int disco::getPlatos(){
     return platos;
 }

 int disco::getSuperficie(){
    return superficies;
 }

int disco::getPistasXsuperficie(){
    return pistasXsuperficie;
}   

int disco::getSectoresXpista(){
    return sectoresXpista;
}

int disco::getTamSector(){
    return tamSector;
}

int disco::getCapacidadDisco(){
    return capacidadDisco;
}
    

     