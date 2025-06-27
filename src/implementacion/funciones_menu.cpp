#include "../include/funciones_menu.h"
#include <cstddef>
#include <iostream>
#include <stdio.h>
#include "../include/funciones.h"
#include "../include/gestorAlmacenamiento.h"
#include "../include/bloque.h"
#include "funcionesBloque.h"
#include "../include/controladorDisco.h"
#include "../include/funciones_menu.h"
#include "../include/funcionesBloque.h"
#include "../include/insertar.h"
#include "../include/bufferManager.h"
#include "gestorBloques.h"
#include <string>
#include <disco.h>
#include <fstream>
using namespace std;

const string ARCHIVO_CSVT = "../../archivos_csv/TitanicG.csv";
const string ARCHIVO_CSVH = "../../archivos_csv/Housing.csv";
const string ARCHIVO_DATOS_T = "../../archivos_esquema/datos_titanic.txt";
const string ARCHIVO_DATOS_H = "../../archivos_esquema/datos_housing.txt";
const string ARCHIVO_LONG_MAX = "../../archivos_esquema/longitudes_maximas.txt";
const string ARCHIVO_ESQUEMAT = "../../archivos_esquema/esquema_titanic.txt";
const string ARCHIVO_ESQUEMAH = "../../archivos_esquema/esquema_hosing.txt";
const string RUTAS = "../../rutas_sectores/rutas.txt";
const string RUTASB = "../../rutas_sectores/cilindroMedio.txt";
const string ARCHIVO_INFO_DISCO = "../../archivo_info_Disco/info_disco.txt";
const string ARCHIVO_B_X_SECTORES = "../../archivo_info_Disco/info_bloque.txt";

void mostrarMenu() {
    cout << "\n----- MEGATROM 3000 -----\n";
    cout << "0. Ya tengo un disco creado \n";
    cout << "1. Crear disco personalizado\n";
    cout << "2. Usar disco por defecto\n";
    cout << "3. Leer archivo CSV y añadir relación\n";
    cout << "4. Caracterisiticas del disco\n";
    cout << "5. inicializar bufferPool\n";
    cout << "6. cargar pagina\n";
    cout << "7. mostrar estado buffer pool (tabla)\n";
    cout << "8. despinear (tabla)\n";
    cout << "10. pinear\n";
    cout << "9. Salir\n";
    cout << "Seleccione una opción: ";
}

void cargarInformacionDisco(gestorAlmacenamiento& gestor, disco& disco1 ){
    string ruta_info_disco = ARCHIVO_INFO_DISCO;
    string ruta_info_SXB = ARCHIVO_B_X_SECTORES;

    ifstream archivo_info_disco(ruta_info_disco);
    ifstream archivo_info_bloque(ruta_info_SXB);

    string linea_info_disco;
    string linea_info_bloque;

    getline(archivo_info_disco,linea_info_disco);
    getline(archivo_info_bloque, linea_info_bloque);

    vector<int> infoDisco = obtenerCamposInt(linea_info_disco);
    vector<int> infoBLoque = obtenerCamposInt(linea_info_bloque);
    imprimirVectorInt(infoDisco);
    imprimirVectorInt(infoBLoque);

    archivo_info_bloque.close();
    archivo_info_disco.close();

    disco1.discoPersonalizado(infoDisco[0], infoDisco[2], infoDisco[3], infoDisco[4]);
    disco1.calcularCapacidadDisco();
    gestor.setDisco(disco1);

    gestor.setSectoresXbloque(infoBLoque[0]);
}

void crearDiscoPersonalizado(gestorAlmacenamiento& gestor, disco& disco1) {
    int platos, pistas, sectores, tamSector, sectoresBloque;
    cout << "Ingrese cantidad de platos: "; cin >> platos;
    cout << "Pistas por superficie: "; cin >> pistas;
    cout << "Sectores por pista: "; cin >> sectores;
    cout << "Tamaño del sector (bytes): "; cin >> tamSector;
    cout << "Sectores por bloque: "; cin >> sectoresBloque;

    disco1.discoPersonalizado(platos, pistas, sectores, tamSector);
    disco1.crearDisco();
    disco1.calcularCapacidadDisco();
    disco1.guardarEnArchivo();

    gestor.setDisco(disco1);
    gestor.setSectoresXbloque(sectoresBloque);
    gestor.guardarEnArchivo();
    gestor.generarArchivoRutas();
    
    
    

    gestor.calcularCapacidadBloque();
    gestor.calcularBloquesXpista();
    gestor.calcularBloquesXplato();
    gestor.calcularBloquesTotales();

    int valor = obtenerRegistroMasLargo(RUTAS, 0);
    gestor.setLongitudRegistroMax(valor);

    gestor.insertarRutasBloques(RUTASB, RUTAS);

    cout << "\n--- Información del disco ---\n";
    cout << "Capacidad: " << disco1.getCapacidadDisco() << " bytes\n";
    cout << "Bloque: " << gestor.getCapacidadBloque() << " bytes\n";
    cout << "Total de bloques: " << gestor.getBloquesTotales() << endl;

}

void usarDiscoPorDefecto(gestorAlmacenamiento& gestor, disco& disco1) {
    const int sectoresBloque = 4;

    disco1.discoEstatico();
    disco1.crearDisco();
    disco1.calcularCapacidadDisco();
    disco1.guardarEnArchivo();
    
    gestor.setDisco(disco1);
    gestor.generarArchivoRutas();

    gestor.setSectoresXbloque(sectoresBloque);

    gestor.calcularCapacidadBloque();
    gestor.calcularBloquesXpista();
    gestor.calcularBloquesXplato();
    gestor.calcularBloquesTotales();

    int valor = obtenerRegistroMasLargo(RUTAS, 0);
    gestor.setLongitudRegistroMax(valor);
    cout << "vLAR" << valor << endl;
    gestor.insertarRutasBloques(RUTASB, RUTAS);
    cout << "\n--- Información del disco ---\n";
    cout << "platos totales: " << disco1.getPlatos() << endl;
    cout << "pistas totales: " << disco1.getPistasXsuperficie() * disco1.getPlatos() * 2 << endl;
    cout << "sectores Totales:" << disco1.getSectoresXpista()*disco1.getPistasXsuperficie()*2*disco1.getPlatos() << endl;
    cout << "Capacidad del disco: " << disco1.getCapacidadDisco() << " bytes\n";
    cout << "Capacidad del bloque: " << gestor.getCapacidadBloque() << " bytes\n";
    cout << "capacidad del sector: " << disco1.getTamSector() << " bytes\n";
    cout << "Total de bloques: " << gestor.getBloquesTotales() << endl;
}

void procesarArchivoCSV(gestorAlmacenamiento& gestor, disco& disco1) {
    int opcionArchivo;
    cout << "Seleccione archivo:\n1. Titanic\n2. Housing\nOpción: ";
    cin >> opcionArchivo;

    string archivo_csv, archivo_datos, esquema, relacion;
    int valor;

    if (opcionArchivo == 1) {
        archivo_csv = ARCHIVO_CSVT;
        archivo_datos = ARCHIVO_DATOS_T;
        esquema = ARCHIVO_ESQUEMAT;
        relacion = "titanic";
        valor = obtenerRegistroMasLargo(archivo_csv, 's');
        gestor.setLongitudRegistroMax(valor);
    } else if (opcionArchivo == 2) {
        archivo_csv = ARCHIVO_CSVH;
        archivo_datos = ARCHIVO_DATOS_H;
        esquema = ARCHIVO_ESQUEMAH;
        relacion = "housing";
        valor = obtenerRegistroMasLargo(archivo_csv, 's');
        gestor.setLongitudRegistroMax(valor);
    } else {
        cout << "Opción no válida.\n";
        return;
    }

    // Generar esquema
    encontrarLongitudesMaximas(archivo_csv, ARCHIVO_LONG_MAX);
    construirEsquema(archivo_csv, archivo_datos, ARCHIVO_LONG_MAX, relacion, esquema);

    // Opciones de inserción
    int opcionInsertar;
    cout << "\n¿Qué desea hacer?\n";
    cout << "1. Insertar 1 registro\n";
    cout << "2. Insertar N registros\n";
    cout << "3. Insertar todo el archivo\n";
    cout << "Opción: ";
    cin >> opcionInsertar;

    switch (opcionInsertar) {
        case 1: {
            insertarRegistroDesdeCSV(gestor, archivo_csv, RUTASB,valor);
            break;
        }
        case 2: {
            cout << "valor: " << valor << endl;
            insertarNRegistrosDesdeCSV(gestor, archivo_csv, RUTASB,valor);
            break;
        }
        case 3:
            insertarTodosLosRegistrosDesdeCSV(gestor, archivo_csv, RUTASB, valor);
            break;

        default:
            cout << "Opción no válida.\n";
    }
}

void caracteristicasDisco(gestorAlmacenamiento& gestor, disco& disco1){
    string ruta_info_disco = ARCHIVO_INFO_DISCO;
    ifstream archivo_info_disco(ruta_info_disco);
    string linea_info_disco;
    getline(archivo_info_disco,linea_info_disco);
    vector<int> infoDisco = obtenerCamposInt(linea_info_disco);
    archivo_info_disco.close();
    cout << "capacidad del disco: " << infoDisco[5] << endl;
    cout << "capacidad libre: "<< gestor.capacidadDisco() << endl;
    cout << "capacidad ocupada del disco: " << infoDisco[5] - gestor.capacidadDisco() << endl;
}

// ... (código existente)

void inicializarBufferPool(bufferManager& bm) {
    int cantFrames;
    cout << "Ingrese cantidad de frames para el buffer pool: ";
    cin >> cantFrames;
    bm = bufferManager(cantFrames);
}

void mostrarEstadoBufferPool(const bufferManager& bm) {
    bm.mostrarEstadoBufferPool();
}

void cargarPagina(bufferManager& bm) {
    int idBloque;
    char modo;
    
    cout << "Ingrese ID del bloque a cargar: ";
    cin >> idBloque;
    
    cout << "Modo de acceso (L para lectura, E para escritura): ";
    cin >> modo;
    modo = toupper(modo);
    
    if (modo != 'L' && modo != 'E') {
        cout << "Modo no válido. Use L (lectura) o E (escritura)." << endl;
        return;
    }
    
    string rutaBloque;
    try {
        rutaBloque = obtenerRutaPorId(RUTASB, idBloque);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return;
    }
    
    bloque b;
    b.inicializarBloque(idBloque, rutaBloque);
    
    string mode_str = (modo == 'L') ? "read" : "write";
    bm.agregarBufferPool(idBloque, b, mode_str);
    
    // Mostrar contenido si es lectura
    if (modo == 'L') {
        cout << "\nContenido del bloque:" << endl;
        b.mostrarBloque();
    }
}

void unpinBlock(bufferManager& bm) {
    int idBloque;
    cout << "Ingrese ID del bloque a despinear: ";
    cin >> idBloque;
    bm.unpinBlock(idBloque);
}

void pinBlock(bufferManager& bm) {
    int idBloque;
    cout << "Ingrese ID del bloque a pinear: ";
    cin >> idBloque;
    bm.pinBlock(idBloque);
}