#ifndef CONTROLADOR_QUERY_H
#define CONTROLADOR_QUERY_H

#include "query.h"
#include "esquema.h"
#include "gestorBloques.h"
#include "bloque.h"
#include "gestorAlmacenamiento.h"
#include "disco.h"


class ControladorQuery{
    private:
    Query query;
    Esquema esquema;
    bloque _bloque;
    gestorBloques gb;
    disco _disco;
    string tipoConsulta;
    public:
    ControladorQuery();
    void generarReporte(Query& query, int id);
};

#endif // !CONTROLADOR_QUERY_H