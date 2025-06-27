#ifndef FUNCIONES_MENU_H
#define FUNCIONES_MENU_H

#include "disco.h"
#include "controladorDisco.h"
#include "gestorAlmacenamiento.h"
#include "bufferManager.h"

void mostrarMenu();
void cargarInformacionDisco(gestorAlmacenamiento& gestor, disco& disco1);
void crearDiscoPersonalizado(gestorAlmacenamiento& gestor, disco& disco1);
void usarDiscoPorDefecto(gestorAlmacenamiento& gestor, disco& disco1);
void procesarArchivoCSV(gestorAlmacenamiento& gestor, disco& disco1);
void caracteristicasDisco(gestorAlmacenamiento& gestor, disco& disco1);
void menuBufferManager(bufferManager& bm);
void seleccionarPoliticaReemplazo(bufferManager& bm);
#endif // FUNCIONES_MENU_H