#ifndef BLOQUE_H
#define BLOQUE_H
#include <string>

using namespace std;

class bloque{
    string cabecera;
    string ruta_bloque;

    private:
        void modificarCabecera(string registro);
};

#endif // !BLOQUE_H
