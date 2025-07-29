#ifndef GESTORBLOQUES_H
#define GESTORBLOQUES_H
#include <unordered_map>
#include "bloque.h"

using namespace std;

class gestorBloques {
    unordered_map<int, bloque> bloques;

    public:
    void agregarBloque(int id, const bloque& b);
    void mostrarBloque(int id) const;
    bloque& getBloque(int id);
    // Añadir en gestorBloques.h

    bloque& obtenerBloque(int id) {
        auto it = bloques.find(id);
        if (it != bloques.end()) {
            return it->second;
        }
        throw std::runtime_error("Bloque no encontrado");
    }


};
#endif // !GESTORBLOQUES_H
//