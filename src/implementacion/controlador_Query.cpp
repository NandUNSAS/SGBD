#include "../include/controlador_Query.h"

void ControladorQuery::realizarConsultaInsert(Query _query, Esquema _esquema){
    //obtengo valores de la cosulta para cargar el esquema
    string nombreTabla = query.getTabla();
    esquema.construirEsquema(nombreTabla);
    
}