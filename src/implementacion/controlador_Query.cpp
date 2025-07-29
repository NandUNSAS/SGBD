#include "../include/controlador_Query.h"
#include "../include/insertar.h"

ControladorQuery::ControladorQuery(){
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

string ControladorQuery::crearRegistro(){
    string registroInsercion;
    return registroInsercion;
}
void ControladorQuery::realizarConsultaInsert(Query _query, int id){
    string nombreTabla = query.getTabla();
    esquema = query.getEsquema();
    _bloque = gb.getBloque(id);
    string registro = crearRegistro();
    agregarInserción(_bloque, registro);
}
