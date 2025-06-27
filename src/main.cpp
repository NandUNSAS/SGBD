#include <cstddef>
#include <iostream>
#include <stdio.h>
#include "include/funciones.h"
#include "include/gestorAlmacenamiento.h"
#include "funcionesBloque.h"
#include "include/controladorDisco.h"
#include "include/bufferManager.h"
#include "include/funciones_menu.h"

#include <string>
#include <disco.h>

#include <fstream>

using namespace std;
#define ARCHIVO_CSVT "/home/asus/Documentos/BD_1/archivos_csv/TitanicG.csv";
#define ARCHIVO_CSVH "/home/asus/Documentos/BD_1/archivos_csv/Housing.csv";

#define ARCHIVO_DATOS_T "/home/asus/Documentos/BD_1/archivos_esquema/datos_titanic.txt";
#define ARCHIVO_DATOS_H "/home/asus/Documentos/BD_1/archivos_esquema/datos_housing.txt";

#define ARCHIVO_LONG_MAX "/home/asus/Documentos/BD_1/archivos_esquema/longitudes_maximas.txt";

#define ARCHIVO_ESQUEMAT "/home/asus/Documentos/BD_1/archivos_esquema/esquema_titanic.txt";
#define ARCHIVO_ESQUEMAH "/home/asus/Documentos/BD_1/archivos_esquema/esquema_hosing.txt"

#define ARCHIVO_PRUEBA "/home/asus/Documentos/BD_1/prueba1.txt"
#define RUTAS "/home/asus/Documentos/BD_1/rutas_sectores/rutas.txt"
#define RUTAS_CILINDRO_MEDIO "/home/asus/Documentos/BD_1/rutas_sectores/cilindroMedio.txt"

int main() {
    controladorDisco cd;
    disco disco1;
    bufferManager bm(0);
    gestorAlmacenamiento gestor(cd);

    int opcion;
    do {
        mostrarMenu();
        cin >> opcion;

        switch (opcion) {
            case 0:
                cargarInformacionDisco(gestor, disco1);
                break;
            case 1:
                crearDiscoPersonalizado(gestor, disco1);
                break;
            case 2:
                usarDiscoPorDefecto(gestor, disco1);
                break;
            case 3:
                procesarArchivoCSV(gestor, disco1);
                break;
            case 4:
                caracteristicasDisco(gestor, disco1);
                break;
            case 5: 
                inicializarBufferPool(bm);
                break;
            case 6:
                cargarPagina(bm);
                break;
            case 7:
                mostrarEstadoBufferPool(bm);
                break;
            case 8: // Nueva opción para despinear
                unpinBlock(bm);
                break;
            case 10:
                pinBlock(bm);
                break;
            case 9:
                cout << "Gracias por usar Megatrom 3000. ¡Hasta luego!\n";
                break;
            default:
                cout << "Opción no válida. Intente nuevamente.\n";
        }

    } while (opcion != 9);

    return 0;
}
//*/