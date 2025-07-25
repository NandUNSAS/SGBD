#ifndef CONTROLADOR_QUERY_H
#define CONTROLADOR_QUERY_H

#include "query.h"
#include "esquema.h"
#include "bufferManager.h"

class ControladorQuery{
    Query query;
    Esquema esquema;
    bufferManager buffer_Manager;

    public:

    void realizarConsultaInsert(Query _query,Esquema _esquema);
};

#endif // !CONTROLADOR_QUERY_H