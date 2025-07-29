#include "../include/insertar.h"
#include "../include/gestorAlmacenamiento.h"
#include "../include/funciones.h"
#include "../include/funcionesBloque.h"
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;
///
string cambiarIndicePorUno(const string& ruta) {
    size_t pos = ruta.find('#');
    if (pos != string::npos && pos > 0) {
        // Verifica si el carácter antes del '#' es un '0'
        if (ruta[pos - 1] == '0') {
            string nuevaRuta = ruta;
            nuevaRuta[pos - 1] = '1';
            return nuevaRuta;
        }
    }
    // Si no se encuentra el '#' o no hay un '0' justo antes, se retorna igual
    return ruta;
}

int bloqueDisponibleInt(string ruta) {  // Cambiamos el tipo de retorno a int
    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << ruta << endl;
        return -1;  // Retorna -1 si hay un error
    }

    string rutaBloque;
    vector<string> campos;
    int bloqueId = 0;  // Contador para el ID del bloque
    
    do {
        if (!getline(archivo, rutaBloque)) {
            cerr << "No se encontraron bloques disponibles" << endl;
            return -1;  // Retorna -1 si no hay bloques disponibles
        }
        bloqueId++;  // Incrementamos el ID cada línea leída
        campos = obtenerCampos(rutaBloque); 
    } while (stoi(campos[0]) == 1);    

    cout << "Bloque disponible encontrado en la línea: " << bloqueId << endl;
    cout << "Ruta del bloque: " << rutaBloque << endl;

    // (El resto del código para encontrar el sector disponible permanece igual)
    ifstream archivoSector(campos[1]);
    string rutaSector;
    vector<string> campos2;
    
    if (getline(archivoSector, rutaSector)) {  // Leer primera línea
        do {
            if (!getline(archivoSector, rutaSector)) {
                cerr << "No se encontraron sectores disponibles" << endl;
                return bloqueId;  // Retorna el bloqueId aunque no haya sector
            }
            campos2 = obtenerCampos(rutaSector);
        } while(stoi(campos2[0]) == 1);
        
        cout << "Sector disponible: " << rutaSector << endl;
    }

    return bloqueId;  // Devolvemos el ID del bloque como int
}

string bloqueDisponible(string ruta){
    ifstream archivo(ruta);

    string rutaBloque;
    int disponible;
    vector<string> campos;
    do {
    getline(archivo, rutaBloque);
    campos = obtenerCampos(rutaBloque); 
    } while (stoi(campos[0]) == 1);    
    cout <<"bloque disponible: " << rutaBloque << endl;

    ifstream archivoSector(campos[1]);

    string rutaSector;
    int disponible1;
    vector<string> campos2;
    getline(archivoSector,rutaSector);
    do {
    getline(archivoSector,rutaSector);
    campos2 = obtenerCampos(rutaSector);
    } while(stoi(campos2[0]) == 1);
    cout << "sector disponible: " << rutaSector << endl;
    
    return campos2[1];
}



void verificadorSectoresLlenos(string ruta) {
    fstream archivo(ruta, ios::in | ios::out);
    if (!archivo.is_open()) {
        cerr << "Error al abrir archivo de bloques: " << ruta << endl;
        return;
    }

    string rutaBloque;
    vector<string> campos;

    // Paso 1: Buscar un bloque activo
    while (getline(archivo, rutaBloque)) {
    campos = obtenerCampos(rutaBloque);
    if (campos.size() < 2) continue;

    if (stoi(campos[0]) == 0) {
        // ¡Este bloque está libre! Queremos trabajar con él.
        break;
    }
    }


    cout << "Bloque para verificar: " << rutaBloque << endl;

    // Paso 2: Abrir archivo de sectores (usar fstream en vez de ifstream)
    fstream archivoSectores(campos[1], ios::in | ios::out);
    if (!archivoSectores.is_open()) {
        cerr << "Error al abrir archivo de sectores: " << campos[1] << endl;
        return;
    }

    string rutaSector;
    vector<string> campos2;
    streampos pos;

    // Paso 3: Saltar cabecera y buscar sector activo
    if (!getline(archivoSectores, rutaSector)) {
        cerr << "Archivo de sectores vacío." << endl;
        return;
    }

    do {
        pos = archivoSectores.tellg();  // Guarda posición antes de leer
        if (!getline(archivoSectores, rutaSector)) {
            cerr << "No se encontró sector disponible." << endl;
            return;
        }
        campos2 = obtenerCampos(rutaSector);
        if (campos2.size() < 2) {
            cerr << "Formato inválido en línea de sector: " << rutaSector << endl;
            return;
        }
    } while (stoi(campos2[0]) != 0);

    //cout << "Sector disponible: " << rutaSector << endl;

    // Paso 4: Abrir archivo del sector
    ifstream archivoSector(campos2[1]);
    if (!archivoSector.is_open()) {
        cerr << "Error al abrir archivo del sector: " << campos2[1] << endl;
        return;
    }

    string cabecera;
    if (!getline(archivoSector, cabecera)) {
        cerr << "No se pudo leer cabecera del sector." << endl;
        return;
    }

    vector<int> campos3 = obtenerCamposInt(cabecera);
    if (campos3.size() < 5) {
        cerr << "Cabecera del sector con formato inválido: " << cabecera << endl;
        return;
    }

    // Paso 5: Verificar si está lleno
    if (campos3[3] == campos3[4]) {
        rutaSector = cambiarIndicePorUno(rutaSector);

        archivoSectores.seekp(pos); // Usar seekp, no seekg
        archivoSectores.write(rutaSector.c_str(), rutaSector.size());
        cout << "Sector marcado como lleno." << endl;
    } else {
        cout << "Sector aún tiene espacio." << endl;
    }
}


void verificadorBloquesLlenos(const string& archivoRutasBloques) {
    fstream archivo(archivoRutasBloques, ios::in | ios::out);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo de rutas de bloques: " << archivoRutasBloques << endl;
        return;
    }

    string linea;
    streampos posInicio;
    while (archivo.peek() != EOF) {
        posInicio = archivo.tellg();  // posición del inicio de la línea
        getline(archivo, linea);
        if (linea.empty()) continue;

        vector<string> campos = obtenerCampos(linea);
        if (campos.size() < 2) continue;

        string estado = campos[0];
        string rutaBloque = campos[1];

        if (estado != "0") continue;  // solo verificamos bloques libres

        ifstream bloque(rutaBloque);
        if (!bloque.is_open()) {
            cerr << "No se pudo abrir bloque: " << rutaBloque << endl;
            continue;
        }

        string rutaSector;
        bool cabeceraSaltada = false;
        bool todosLlenos = true;

        while (getline(bloque, rutaSector)) {
            if (!cabeceraSaltada) {
                cabeceraSaltada = true;  // omitir primera línea
                continue;
            }
            if (rutaSector.substr(0, 2) == "0#") {
                todosLlenos = false;
                break;
            }
        }

        if (todosLlenos) {
            string nuevaLinea = cambiarIndicePorUno(linea);
            archivo.clear();  // limpiar estado anterior
            archivo.seekp(posInicio);
            archivo.write(nuevaLinea.c_str(), nuevaLinea.size());
            archivo.flush();
            cout << "Bloque lleno marcado: " << nuevaLinea << endl;
        }
    }
}


void insertarRegistroDesdeCSV(gestorAlmacenamiento& gestor, const string& archivo_csv, const string& rutaBloques, int tam) {
    int filaDeseada;
    cout << "Ingrese el número de fila del registro a insertar: ";
    cin >> filaDeseada;

    ifstream archivo(archivo_csv);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo CSV: " << archivo_csv << endl;
        return;
    }

    string linea;
    int filaActual ;  // -1 porque ignoramos encabezado

    // Buscar la fila deseada
    while (getline(archivo, linea)) {
        if (filaActual == filaDeseada) break;
        filaActual++;
    }

    if (filaActual < filaDeseada) {
        cerr << "La fila " << filaDeseada << " no existe en el archivo." << endl;
        return;
    }

    // Ahora `linea` contiene el registro deseado
    string rutaSector = bloqueDisponible(rutaBloques);
    if (rutaSector.empty()) {
        cerr << "No se encontró un sector disponible." << endl;
        return;
    }

    // O puedes usar tu propio cálculo
    gestor.insertarRegistro(linea, rutaSector, tam);

    cout << "Registro insertado correctamente:\n" << linea << endl;
}


void insertarNRegistrosDesdeCSV(gestorAlmacenamiento& gestor, const string& archivo_csv, const string& rutaBloques, int tam) {
    int cantidad;
    cout << "¿Cuántos registros desea insertar desde el archivo CSV? ";
    cin >> cantidad;

    if (cantidad <= 0) {
        cerr << "La cantidad debe ser mayor que cero." << endl;
        return;
    }

    ifstream archivo(archivo_csv);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo CSV: " << archivo_csv << endl;
        return;
    }

    string linea;
    int contador = 0;

    // Saltar la cabecera
    if (!getline(archivo, linea)) {
        cerr << "Archivo CSV vacío o sin cabecera." << endl;
        return;
    }

    while (getline(archivo, linea) && contador < cantidad) {
        cout << endl;
        cout << "------------------------------------------------------------------\n";
        string rutaSector = bloqueDisponible(rutaBloques);
        if (rutaSector.empty()) {
            cerr << "No se encontró un sector disponible para el registro #" << contador + 1 << endl;
            break;
        }
        
        // Tamaño simple, puedes cambiar si usas longitud fija
        gestor.insertarRegistro(linea, rutaSector, tam);

        cout << "Registro #" << contador + 1 << " insertado: " << linea << endl;
        contador++;
    }

    if (contador < cantidad) {
        cout << "Solo se insertaron " << contador << " registros. Puede que no haya sectores disponibles suficientes." << endl;
    } else {
        cout << "Se insertaron los " << cantidad << " registros correctamente." << endl;
    }
}


void insertarTodosLosRegistrosDesdeCSV(gestorAlmacenamiento& gestor, const string& archivo_csv, const string& rutaBloques, int tam) {
    ifstream archivo(archivo_csv);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo CSV: " << archivo_csv << endl;
        return;
    }

    string linea;
    int contador = 0;

    // Saltar cabecera
    if (!getline(archivo, linea)) {
        cerr << "Archivo CSV vacío o sin cabecera." << endl;
        return;
    }

    // Leer e insertar todos los registros
    while (getline(archivo, linea)) {
        string rutaSector = bloqueDisponible(rutaBloques);
        if (rutaSector.empty()) {
            cerr << "No se encontró un sector disponible para el registro #" << contador + 1 << endl;
            break;
        }

         // o calcula de otra forma si necesitas longitud fija
        gestor.insertarRegistro(linea, rutaSector, tam);

        cout << "Registro #" << contador + 1 << " insertado: " << linea << endl;
        contador++;
    }

    cout << "Total de registros insertados: " << contador << endl;
}