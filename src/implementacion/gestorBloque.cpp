#include "../include/gestorBloques.h"
#include <iostream>

using namespace std;


void gestorBloques::agregarBloque(int id, const bloque& b) {
    bloques[id] = b;
}


void gestorBloques::mostrarBloque(int id) const{
    auto it = bloques.find(id);
        if (it != bloques.end()) {
            const bloque& b = it->second;
            b.mostrarBloque();
        } else {
            cout << "Bloque con ID " << id << " no encontrado." << endl;
        }
}

bloque& gestorBloques::getBloque(int id) {
    auto it = bloques.find(id);
    if (it != bloques.end()) {
        return it->second;  // Devuelve referencia al bloque existente
    } else {
        throw std::out_of_range("Bloque con ID " + std::to_string(id) + " no encontrado.");
    }
}
//
