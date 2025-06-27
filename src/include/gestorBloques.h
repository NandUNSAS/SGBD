#ifndef GESTORBLOQUES_H
#define GESTORBLOQUES_H
#include <unordered_map>
#include "bloque.h"

using namespace std;

class gestorBloques {
    unordered_map<int, bloque> bloques;

    public:
    void agregarBloque(int id, const bloque& b);
    void mostrarBloque(int id);
};
#endif // !GESTORBLOQUES_H
//