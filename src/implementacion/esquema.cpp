#include "../include/esquema.h"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

#define ESQUEMAS "../../archivos_esquema/esquema_"

void Esquema::construirEsquema(const string& archivoEsquema) {
    ifstream archivo(ESQUEMAS + archivoEsquema +".txt");
    
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo de esquema: " << ESQUEMAS + archivoEsquema +".txt" << endl;
        return;
    }

    string linea;
    getline(archivo, linea);
    vector<string> bloques;
    
    // Dividir la línea por #
    size_t pos = 0;
    string token;
    while ((pos = linea.find('#')) != string::npos) {
        token = linea.substr(0, pos);
        bloques.push_back(token);
        linea.erase(0, pos + 1);
    }
    bloques.push_back(linea); // Agregar el último bloque

    if (bloques.size() < 4 || (bloques.size() - 1) % 3 != 0) {
        cerr << "Formato de esquema inválido" << endl;
        return;
    }

    // El primer bloque es el nombre de la tabla
    nombreTabla = bloques[0];
    
    // Procesar campos (bloques de 3 en 3)
    int campoActual = 0;
    for (size_t i = 1; i < bloques.size(); i += 3) {
        if (i + 2 >= bloques.size()) break;
        
        CampoEsquema campo;
        campo.nombre = bloques[i];
        campo.tipo = bloques[i+1];
        campo.tamano = stoi(bloques[i+2]);
        campo.posicion = campoActual++;
        
        campos.push_back(campo);
    }
}

void Esquema::setNombreTabla(const string& nombre) {
    nombreTabla = nombre;
}

string Esquema::getNombreTabla() const {
    return nombreTabla;
}

int Esquema::buscarPosicionCampo(const string& nombreCampo) const {
    auto it = find_if(campos.begin(), campos.end(), 
        [&nombreCampo](const CampoEsquema& c) { return c.nombre == nombreCampo; });
    
    if (it != campos.end()) {
        return it->posicion;
    }
    return -1; // No encontrado
}

vector<int> Esquema::buscarPosicionesCampos(const vector<string>& nombresCampos) const {
    vector<int> posiciones;
    
    for (const auto& nombre : nombresCampos) {
        int pos = buscarPosicionCampo(nombre);
        posiciones.push_back(pos);
    }
    
    return posiciones;
}

void Esquema::mostrarEsquema() const {
    cout << "Esquema de tabla '" << nombreTabla << "' (0-based):" << endl;
    for (const auto& campo : campos) {
        cout << campo.posicion << ": " << campo.nombre 
             << " (" << campo.tipo << ", tamaño: " << campo.tamano << ")" << endl;
    }
}

const vector<CampoEsquema>& Esquema::getCampos() const {
    return campos;
}

int Esquema::calcularTamanoRegistro() {
    int total = 0;
    for (const auto& campo : campos) {
        total += campo.tamano;
    }

    // Agregar el tamaño de los separadores '#'
    if (!campos.empty()) {
        total += campos.size() - 1;
    }

    return total;
}

/*
string crearArchivoIndexacion(int numeroCampo){

}
string generarEntradaArchivoIndexacion(Query& query, bool _todo_campos, string archivoEsquema){
    bool _todos_campos = query.getTodos_Campos();
    vector<CampoEsquema> esquema = procesarEsquema(archivoEsquema);
    string atributo_where = query.getAtributo();
    int posicion_atributo_where = buscarPosicionCampo(esquema, atributo_where);
}*/