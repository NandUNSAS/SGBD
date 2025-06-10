#ifndef FUNCIONES_MENU_H
#define FUNCIONES_MENU_H
#include "disco.h"
#include "controladorDisco.h"
#include "gestorAlmacenamiento.h"

void mostrarMenu();
void cargarInformacionDisco(gestorAlmacenamiento& gestor, disco& disco1);
void crearDiscoPersonalizado(gestorAlmacenamiento& gestor, disco& disco1);
void usarDiscoPorDefecto(gestorAlmacenamiento& gestor, disco& disco1);
void procesarArchivoCSV(gestorAlmacenamiento& gestor, disco& disco1);
void caracteristicasDisco(gestorAlmacenamiento& gestor, disco& disco1);

#endif // !FUNCIONES_MENU_H