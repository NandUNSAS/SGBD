#include "../include/controladorDisco.h"
#include "../include/funcionesBloque.h"
#include <iosfwd>
#include <string>
#include <fstream>
//
using namespace std;

void controladorDisco::insertar(string registro,int tamaño_fijo_registro, string rutaSector){
    ofstream _rutaSector(rutaSector, ios::app);
    _rutaSector << registro << '\n';
    _rutaSector.close();
}

//void controladorDisco::insertarS(string registro, streampos pos,string rutaSector)

