#ifndef INSERTAR_H
#define INERTAR_H
#include "../include/gestorAlmacenamiento.h"
#include "../include/funciones.h"
#include "../include/funcionesBloque.h"


void bloqueDisponible();
void insertarRegistroDesdeCSV(gestorAlmacenamiento& gestor, const string& archivo_csv, const string& rutaBloques,int tam);
void insertarNRegistrosDesdeCSV(gestorAlmacenamiento& gestor, const string& archivo_csv, const string& rutaBloques, int tam);
void insertarTodosLosRegistrosDesdeCSV(gestorAlmacenamiento& gestor, const string& archivo_csv, const string& rutaBloques, int tam);
string bloqueDisponible(string ruta);
void verificadorSectoresLlenos(string ruta);
void verificadorBloquesLlenos(const string& archivoRutasBloques);

#endif // !

//