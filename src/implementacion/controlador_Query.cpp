#include "../include/controlador_Query.h"
#include "../include/insertar.h"
#include <sstream>
#include <iomanip>
#include <fstream>


ControladorQuery::ControladorQuery(){

}

void ControladorQuery::setTipoConsulta(string& tipo){
    tipoConsulta = tipo;
}

int obtenerIdBloqueVacio(const string& rutaArchivo) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << rutaArchivo << endl;
        return -1;  // Valor de error
    }

    string linea;
    int id = 1;  // Empezamos desde 1

    while (getline(archivo, linea)) {
        if (linea.empty()) {
            ++id;
            continue;
        }

        size_t pos = linea.find('#');
        if (pos != string::npos) {
            string estadoStr = linea.substr(0, pos);
            if (estadoStr == "0") {
                archivo.close();
                return id;
            }
        }

        ++id;
    }

    archivo.close();
    return -1;  // No se encontró ningún bloque libre
}

void ControladorQuery::efectuarConsulta(Query& _query, string rutaBloques){
    if(tipoConsulta == "INSERT"){
        int id = obtenerIdBloqueVacio(rutaBloques);
        realizarConsultaInsert(_query, id);
    }
}

void modifcarCabecera(string& cabecera, int tamañoRegistro, int cantidadMaximaRegistros){

}

void obtenerDesplazamientoInserción(){

}
string obtenerCabecera(string& contenidoBloque){
    return contenidoBloque;
}

int obtenerDesplazamiento(string& cabecera){
    int desplazamiento = 0;
    return desplazamiento;
}

void desplazamientoInserción(int desplazamiento){

}
void ControladorQuery::agregarInserción(bloque& _bloque, string& registro){
    string contenidoBloque = _bloque.getContenido();
    string cabecera = obtenerCabecera(contenidoBloque);
    esquema = query.getEsquema();
    int tamañoRegistro = esquema.calcularTamanoRegistro();
    int cantidadMaximaRegistros = tamañoRegistro/tamañoRegistro;
    modifcarCabecera(cabecera, tamañoRegistro, cantidadMaximaRegistros);
    int desplazamiento = obtenerDesplazamiento(cabecera);
    desplazamientoInserción(desplazamiento);
}

string ControladorQuery::crearRegistro() {
    const vector<CampoEsquema>& campos = query.getEsquema().getCampos();
    const vector<string>& valores = query.getValores();
    string registroInsercion;

    if (valores.size() != campos.size()) {
        cerr << "Error: cantidad de valores no coincide con el esquema." << endl;
        return "";
    }

    for (size_t i = 0; i < campos.size(); ++i) {
        string valor = valores[i];
        string tipo = campos[i].tipo;
        int tamano = campos[i].tamano;

        if (tipo == "string") {
            if ((int)valor.size() > tamano) {
                valor = valor.substr(0, tamano);  // Truncar
            } else {
                valor.append(tamano - valor.size(), ' ');  // Padding con espacios
            }
        } else if (tipo == "int") {
            valor = to_string(stoi(valor));
            if ((int)valor.size() > tamano) {
                valor = valor.substr(0, tamano);
            } else {
                valor.insert(valor.begin(), tamano - valor.size(), '0');
            }
        } else if (tipo == "float") {
            stringstream ss;
            ss << fixed << setprecision(2) << stof(valor);
            valor = ss.str();
            if ((int)valor.size() > tamano) {
                valor = valor.substr(0, tamano);
            } else {
                valor.insert(valor.begin(), tamano - valor.size(), '0');
            }
        }

        registroInsercion += valor;

        // Agregar '#' solo si no es el último campo
        if (i != campos.size() - 1) {
            registroInsercion += '#';
        }
    }

    return registroInsercion;
}



void ControladorQuery::realizarConsultaInsert(Query _query, int id){
    //string nombreTabla = query.getTabla();
    esquema = query.getEsquema();
    _bloque = gb.getBloque(id);
    string registro = crearRegistro();
    agregarInserción(_bloque, registro);
}
