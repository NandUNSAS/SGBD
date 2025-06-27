#ifndef CONTROLADORDISCO_H
#define CONTROLADORDISCO_H
#include <iosfwd>
#include <string>
using namespace std;

class controladorDisco{
    public:
        void insertar(string registro,int tamaño_fijo_registro, string rutaSector);
};


#endif // !CONTROLADO
//