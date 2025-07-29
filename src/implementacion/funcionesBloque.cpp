#include "../include/funcionesBloque.h"
#include <cstdio>
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

#define ARCHIVO_ESQUEMA_HEAD "../../archivos_esquema/esquema_registro_bloques.txt"

using namespace std;

void imprimirVectorString(const vector<string>& vec) {
    for (const string& valor : vec) {
        cout << valor << " ";
    }
    cout << endl;
}

void imprimirVectorInt(const vector<int>& vec) {
    for (int valor : vec) {
        cout << valor << " ";
    }
    cout << endl;
}


std::string limpiarYConvertir(const std::string& entrada) {
    std::string resultado;

    for (char c : entrada) {
        // Respeta el salto de línea
        if (c == '\n') {
            resultado += c;
        }
        // Elimina retorno de carro u otros caracteres no imprimibles (excepto \n)
        else if (c == '\r' || !isprint(c)) {
            continue;
        }
        // Reemplaza caracteres alfanuméricos por '#'
        else if (c == ',') {
            resultado += '#';
        }
        // Conserva otros símbolos visibles
        else {
            resultado += c;
        }
    }

    return resultado;
}

vector<string> obtenerCampos(const string& linea) {
    vector<string> campos;
    stringstream ss(linea);
    string campo;

    while (getline(ss, campo, '#')) {
        campos.push_back(campo);
    }

    return campos;
}

vector<int> convertirVectorAEnteros(const vector<string>& strVector) {
    vector<int> intVector;
    for (const string& s : strVector) {
        try {
            intVector.push_back(stoi(s));
        } catch (const invalid_argument& e) {
            // La cadena no es un número válido, puedes registrar el error si lo deseas
        } catch (const out_of_range& e) {
            // El número está fuera del rango de int, puedes registrar el error si lo deseas
        }
    }
    return intVector;
}

vector<int> obtenerCamposInt(const string& linea) {
    vector<int> campos;
    stringstream ss(linea);
    string campo;

    while (getline(ss, campo, '#')) {
        try {
            campos.push_back(stoi(campo));
        } catch (const invalid_argument& e) {
            // Si el campo no es un número, puedes ignorarlo o poner un valor por defecto
            campos.push_back(0); // o continuar; si prefieres ignorar el campo
        } catch (const out_of_range& e) {
            campos.push_back(0); // o continuar;
        }
    }

    return campos;
}

streampos calcularOffsetCampo(const vector<int>& tamanos, int indice) {
    streampos offset = 0;
    for (int i = 0; i < indice; ++i) {
        offset += tamanos[i] + 1; // +1 por el caracter '#'
    }
    return offset;
}

bool modificarCampoCabecera(fstream& archivo, int indice, int nuevoValor, const vector<int>& tamanos) {
    streampos offset = calcularOffsetCampo(tamanos, indice);
    //cout << "offset" << offset << endl;
    archivo.seekp(offset);

    string valor_str = to_string(nuevoValor);

    // Rellenar con ceros a la izquierda
    while (valor_str.length() < static_cast<size_t>(tamanos[indice])) {
        valor_str = "0" + valor_str;
    }

    if (valor_str.length() > static_cast<size_t>(tamanos[indice])) {
        cerr << "Error: El valor excede el espacio reservado en el campo " << indice << endl;
        return false;
    }
    archivo.write(valor_str.c_str(), tamanos[indice]);
    return true;
}

bool actualizarCabecera(int tamanoRegistro, string ruta_sector, int capacidadSector) {
    fstream archivo_sector(ruta_sector, ios::in | ios::out);
    if (!archivo_sector.is_open()) {
        cerr << "No se pudo abrir el archivo: " << ruta_sector << "\n";
        return 0;
    }

    ifstream archivo_head_bloque(ARCHIVO_ESQUEMA_HEAD);
    if (!archivo_head_bloque.is_open()) {
        cerr << "No se pudo abrir el archivo de esquema.\n";
        return 0;
    } 
    // obtener el la linea del esquema cabecera(numeros de los espacios reservados del head) 
    //guardarlo en un vector de string y luego guardarlo en vector de int.
    string esquema_cabecera;
    getline(archivo_head_bloque, esquema_cabecera);
    vector<string> campos_esquema = obtenerCampos(esquema_cabecera);
    vector<int> tamanos;
    for (const string& campo : campos_esquema) {
        tamanos.push_back(stoi(campo));
    }

    //obtener en un vector el head de un sector.
    string linea_cabecera_sector;
    getline(archivo_sector, linea_cabecera_sector);
    vector<string> campos_cabecera = obtenerCampos(linea_cabecera_sector);

    vector<int> valores_int(5);
    for (int i = 0; i < 5; ++i) {
        valores_int[i] = stoi(campos_cabecera[i]);
    }
    //cout << capacidadSector <<" | "<< tamanoRegistro << endl;
    // Actualizar valores necesarios
    valores_int[0] += tamanoRegistro; 
    valores_int[2] = tamanoRegistro;
    valores_int[3] += 1;
    valores_int[4] = (capacidadSector - 17) / tamanoRegistro;
    
    //imprimirVectorInt(valores_int);
    // Reescribir solo los campos actualizados
    if(valores_int[3] > valores_int[4]){
       // cout << valores_int[3] <<" | "  << valores_int[4] << endl;
        cout << "bloque ya no puede insertar mas valores"<< endl;
        return false;

    }
    else {
        if (!modificarCampoCabecera(archivo_sector, 0, valores_int[0], tamanos)) return false;
        if (!modificarCampoCabecera(archivo_sector, 2, valores_int[2], tamanos)) return false;
        if (!modificarCampoCabecera(archivo_sector, 3, valores_int[3], tamanos)) return false;
        if (!modificarCampoCabecera(archivo_sector, 4, valores_int[4], tamanos)) return false;
        return true;

    }
    archivo_sector.close();
    return false;
}


bool actualizarCabeceraB(int tamanoRegistro, string ruta_sector, int capacidadSector, int sectoresXbloque) {
    fstream archivo_sector(ruta_sector, ios::in | ios::out);
    if (!archivo_sector.is_open()) {
        cerr << "No se pudo abrir el archivo: " << ruta_sector << "\n";
        return 0;
    }

    ifstream archivo_head_bloque(ARCHIVO_ESQUEMA_HEAD);
    if (!archivo_head_bloque.is_open()) {
        cerr << "No se pudo abrir el archivo de esquema.\n";
        return 0;
    } 
    // obtener el la linea del esquema cabecera(numeros de los espacios reservados del head) 
    //guardarlo en un vector de string y luego guardarlo en vector de int.
    string esquema_cabecera;
    getline(archivo_head_bloque, esquema_cabecera);
    vector<string> campos_esquema = obtenerCampos(esquema_cabecera);
    vector<int> tamanos;
    for (const string& campo : campos_esquema) {
        tamanos.push_back(stoi(campo));
    }

    //obtener en un vector el head de un sector.
    string linea_cabecera_sector;
    getline(archivo_sector, linea_cabecera_sector);
    vector<string> campos_cabecera = obtenerCampos(linea_cabecera_sector);

    vector<int> valores_int(5);
    for (int i = 0; i < 5; ++i) {
        valores_int[i] = stoi(campos_cabecera[i]);
    }
   //imprimirVectorInt(valores_int);
    //cout << capacidadSector <<" | "<< tamanoRegistro << endl;
    // Actualizar valores necesarios
    valores_int[0] += tamanoRegistro; 
    valores_int[2] = tamanoRegistro;
    valores_int[3] += 1;
    valores_int[4] = sectoresXbloque;
    
    imprimirVectorInt(valores_int);
    // Reescribir solo los campos actualizados
    if(valores_int[3] > valores_int[4]){
        //cout << valores_int[3] <<" | "  << valores_int[4] << endl;
        cout << "bloque ya no puede insertar mas valores"<< endl;
        return false;

    }
    else {
        if (!modificarCampoCabecera(archivo_sector, 0, valores_int[0], tamanos)) return false;
        if (!modificarCampoCabecera(archivo_sector, 2, valores_int[2], tamanos)) return false;
        if (!modificarCampoCabecera(archivo_sector, 3, valores_int[3], tamanos)) return false;
        if (!modificarCampoCabecera(archivo_sector, 4, valores_int[4], tamanos)) return false;
        return true;

    }
    archivo_sector.close();
    return false;
}
//
