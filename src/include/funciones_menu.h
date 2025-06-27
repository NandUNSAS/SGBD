#ifndef FUNCIONES_MENU_H
#define FUNCIONES_MENU_H

#include "disco.h"
#include "controladorDisco.h"
#include "gestorAlmacenamiento.h"
#include "bufferManager.h"  // Añade esta línea

void mostrarMenu();
void cargarInformacionDisco(gestorAlmacenamiento& gestor, disco& disco1);
void crearDiscoPersonalizado(gestorAlmacenamiento& gestor, disco& disco1);
void usarDiscoPorDefecto(gestorAlmacenamiento& gestor, disco& disco1);
void procesarArchivoCSV(gestorAlmacenamiento& gestor, disco& disco1);
void caracteristicasDisco(gestorAlmacenamiento& gestor, disco& disco1);

// Añade estas nuevas declaraciones
void inicializarBufferPool(bufferManager& bm);
void cargarPagina(bufferManager& bm);
void mostrarEstadoBufferPool(const bufferManager& bm);
void unpinBlock(bufferManager& bm);
void pinBlock(bufferManager& bm);

#endif // !FUNCIONES_MENU_H