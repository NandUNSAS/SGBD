#ifndef  QUERY_H
#define QUERY_H

#include <string>
#include <vector>

using namespace std;

class Query {
private:
    string tipo;               // SELECT, INSERT, DELETE, UPDATE
    string tabla;
    bool tieneWhere;

    // Para SELECT, DELETE, UPDATE
    string atributo;
    string valor;

    // Para INSERT
    vector<string> columnas;
    vector<string> valores;

    // Para UPDATE
    string atributoUpdate;
    string nuevoValor;

public:
    Query();

    void pedirTipoConsulta();
    void pedirDatos();

    string getTipo() const;
    string getTabla() const;
    bool getTieneWhere() const;
    string getAtributo() const;
    string getValor() const;

    void ejecutarSelect(const vector<string>& registros) const;
    void ejecutarInsert(vector<string>& registros) const;
    void ejecutarDelete(vector<string>& registros) const;
    void ejecutarUpdate(vector<string>& registros) const;
};

#endif // DEBUG
//