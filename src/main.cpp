#include <cstddef>
#include <iostream>
#include <stdio.h>
#include "include/funciones.h"
#include "include/gestorAlmacenamiento.h"
#include "funcionesBloque.h"
#include "include/controladorDisco.h"
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
/*char leerCaracterEnOffset(const string& rutaArchivo, streampos offset) {
    ifstream archivo(rutaArchivo, ios::binary);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo." << endl;
        return '\0'; // o algún valor que indique error
    }

    // Mover el puntero de lectura al offset indicado
    archivo.seekg(offset, ios::beg);
    if (!archivo.good()) {
        cerr << "Error al posicionar el puntero en el archivo." << endl;
        return '\0';
    }

    char caracter;
    archivo.get(caracter);
    if (archivo.eof()) {
        cerr << "Se llegó al final del archivo, no hay carácter en esa posición." << endl;
        return '\0';
    }

    return caracter;
}

int main() {
    string ruta = "/home/asus/Documentos/BD_1/DiscoLocal/plato1/superficie1/pista5/sector1.txt";
    streampos offset = 333; // posición que quieres leer

    char c = leerCaracterEnOffset(ruta, offset);
    if (c != '\0') {
        cout << "El carácter en la posición " << offset << " es: '" << c << "'" << endl;
    } else {
        cout << "No se pudo leer el carácter." << endl;
    }

    return 0;
}
///*
void insertar(string registro,int tamaño_fijo_registro, string rutaSector){
    ofstream _rutaSector(rutaSector, ios::app);
    _rutaSector << registro << '\n';
    _rutaSector.close();
}
/*/
int main() {
    controladorDisco cd;
    disco disco1;
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
            case 8:
                cout << "Gracias por usar Megatrom 3000. ¡Hasta luego!\n";
                break;
            default:
                cout << "Opción no válida. Intente nuevamente.\n";
        }

    } while (opcion != 8);

    return 0;
}
//*/