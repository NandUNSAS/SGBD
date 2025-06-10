#include <iostream>
#include <fstream>
#include "include/funcionesBloque.h"
using namespace std;

char leerCaracterDesdeSeek(const string& rutaArchivo, int offset) {
    ifstream archivo(rutaArchivo, ios::in | ios::binary);

    if (!archivo) {
        cerr << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return '\0'; // Valor nulo si falla
    }

    archivo.seekg(offset, ios::beg); // Desplazar desde el inicio
    if (!archivo) {
        cerr << "Error al hacer seek en el archivo." << endl;
        return '\0';
    }

    char c;
    archivo.get(c); // Leer el carácter

    if (!archivo) {
        cerr << "Error al leer el carácter." << endl;
        return '\0';
    }

    return c;
}



int main() {
    string archivo = "/home/asus/Documentos/BD_1/DiscoLocal/plato3/superficie1/pista5/sector1.txt";
    int numero = devolverCampo(archivo,1);
    cout << numero <<endl;
    

    
    //int offset = 18; // posición desde donde leer

    //char valor = leerCaracterDesdeSeek(archivo, offset);
    //cout << "Carácter en offset " << offset << ": " << valor << endl;

    return 0;
}
