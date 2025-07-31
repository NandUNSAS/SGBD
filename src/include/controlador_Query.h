#ifndef CONTROLADOR_QUERY_H
#define CONTROLADOR_QUERY_H

#include "query.h"
#include "esquema.h"
#include "gestorBloques.h"
#include "bloque.h"
#include "gestorAlmacenamiento.h"
#include "disco.h"
#include "../include/hashing_extendible.h"

class ControladorQuery{
    private:
    ExtendibleHash postulantes_index;
    Query query;
    Esquema esquema;
    bloque _bloque;
    gestorBloques gb;
    disco _disco;
    string tipoConsulta;
    public:
    void generarIndicePostulantes();
    std::string buscarPostulantePorID(int id);
    void actualizarIndiceInsercion(int id, int block_id, int offset);
    ControladorQuery();
    void realizarConsultaInsert(Query _query,int id);
    void agregarInserción(bloque& _bloque, string& registro);
    void efectuarConsulta(Query& _query, string rutaBloques);
    string crearRegistro();
    void setTipoConsulta(string& tipo);
};

#endif // !CONTROLADOR_QUERY_H