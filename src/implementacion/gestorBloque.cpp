#include "../include/gestorBloques.h"
#include <iostream>

using namespace std;


void gestorBloques::agregarBloque(int id, const bloque& b) {
    bloques[id] = b;
}


void gestorBloques::mostrarBloque(int id){
    auto it = bloques.find(id);
        if (it != bloques.end()) {
            const bloque& b = it->second;
            b.mostrarBloque();
        } else {
            cout << "Bloque con ID " << id << " no encontrado." << endl;
        }
}
//
