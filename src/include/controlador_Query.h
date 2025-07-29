#ifndef CONTROLADOR_QUERY_H
#define CONTROLADOR_QUERY_H

#include "query.h"
#include "esquema.h"
#include "gestorBloques.h"
#include "bloque.h"

class ControladorQuery{
    Query query;
    Esquema esquema;
    bloque _bloque;
    gestorBloques gb;
    public:
    ControladorQuery();
    void realizarConsultaInsert(Query _query,int id);
    void agregarInserción(bloque& _bloque, string& registro);
    string crearRegistro();
};



#endif // !CONTROLADOR_QUERY_H