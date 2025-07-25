#ifndef  QUERY_H
#define QUERY_H

#include <string>
#include <vector>
#include "esquema.h"
using namespace std;

class Query {
private:
    string tipo;               // SELECT, INSERT, DELETE, UPDATE
    string tabla;
    bool tieneWhere;
    vector<string> camposSelect; 
    string valorMinimo;          // Para rangos (BETWEEN)
    string valorMaximo;          // Para rangos (BETWEEN)
    int opcionCondicion;
    bool todos_campos; 

    // Para SELECT, DELETE, UPDATE
    string atributo;
    string valor;

    // Para INSERT
    vector<string> columnas;
    vector<string> valores;

    // Para UPDATE
    string atributoUpdate;
    string nuevoValor;

    //ESQUEMA
    Esquema esquema;
public:
    Query();
    

    void pedirTipoConsulta();
    void pedirDatos();

    void pedirCamposSelect();
    void pedirWhereSelect();
    void pedirWhereBasico();
    void pedirDatosUpdate();
    void pedirDatosInsert();

    string getTipo() const;
    string getTabla() const;
    bool getTieneWhere() const;
    string getAtributo() const;
    string getValor() const;
    int getOpcionCondicion() const;
    bool getTodos_Campos() const;

    void setEsquema(Esquema& esquema);
    string generarConsultaSQL() const;
    void imprimirConsulta() const;
    
};

#endif // DEBUG
//