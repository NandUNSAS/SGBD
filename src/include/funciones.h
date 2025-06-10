#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <string>
using namespace std;


void añadirEsquema(const string& relacion,const string& archivo_cav);
size_t obtenerRegistroMasLargo(const string& nombreArchivo,char c);
void encontrarLongitudesMaximas(const string& archivoEntrada, const string& archivoSalida);
void construirEsquema(const string& archivoAtributos,
                      const string& archivoTipos,
                      const string& archivoLongitudes,
                      const string& nombreRelacion,
                      const string& archivoSalida);

#endif 

