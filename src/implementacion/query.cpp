#include "../include/query.h"
#include <iostream>

using namespace std;

Query::Query() : tipo("UNDEFINED"), tieneWhere(false) {}

void Query::pedirTipoConsulta() {
    int opcion;
    cout << "\n----- TIPO CONSULTA -----\n";
    cout << "1. SELECT\n";
    cout << "2. INSERT\n";
    cout << "3. DELETE\n";
    cout << "4. UPDATE\n";
    cin >> opcion;

    switch (opcion) {
        case 1: tipo = "SELECT"; break;
        case 2: tipo = "INSERT"; break;
        case 3: tipo = "DELETE"; break;
        case 4: tipo = "UPDATE"; break;
        default: tipo = "UNKNOWN";
    }
}

void Query::pedirDatos() {
    cout << "Introduce el nombre de la tabla: ";
    cin >> tabla;

    if (tipo == "SELECT" || tipo == "DELETE" || tipo == "UPDATE") {
        cout << "¿Deseas aplicar cláusula WHERE? (1: Sí, 0: No): ";
        int opcion;
        cin >> opcion;
        tieneWhere = (opcion == 1);

        if (tieneWhere) {
            cout << "Introduce el nombre del atributo (columna): ";
            cin >> atributo;
            cout << "Introduce el valor que debe tener ese atributo: ";
            cin >> valor;
        }

        if (tipo == "UPDATE") {
            cout << "Introduce el nombre del atributo que deseas modificar: ";
            cin >> atributoUpdate;
            cout << "Introduce el nuevo valor para ese atributo: ";
            cin >> nuevoValor;
        }
    }
    else if (tipo == "INSERT") {
        int n;
        cout << "¿Cuántas columnas deseas insertar?: ";
        cin >> n;
        columnas.resize(n);
        valores.resize(n);

        for (int i = 0; i < n; ++i) {
            cout << "Nombre de la columna #" << (i + 1) << ": ";
            cin >> columnas[i];
            cout << "Valor para " << columnas[i] << ": ";
            cin >> valores[i];
        }
    }
}

string Query::getTipo() const {
    return tipo;
}

string Query::getTabla() const {
    return tabla;
}

bool Query::getTieneWhere() const {
    return tieneWhere;
}

string Query::getAtributo() const {
    return atributo;
}

string Query::getValor() const {
    return valor;
}
/*
void Query::ejecutarSelect(const vector<string>& registros) const {

}

void Query::ejecutarInsert(vector<string>& registros) const {
    string nuevoRegistro;
    for (size_t i = 0; i < columnas.size(); ++i) {
        nuevoRegistro += valores[i];
        if (i < columnas.size() - 1)
            nuevoRegistro += ",";
    }
    registros.push_back(nuevoRegistro);
    cout << "Registro insertado: " << nuevoRegistro << endl;
}

void Query::ejecutarDelete(vector<string>& registros) const {
    vector<string> resultado;
    for (const auto& reg : registros) {
        if (!cumpleCondicion(reg)) {
            resultado.push_back(reg);
        } else {
            cout << "Registro eliminado: " << reg << endl;
        }
    }
    registros = resultado;
}

void Query::ejecutarUpdate(vector<string>& registros) const {
    for (auto& reg : registros) {
        if (cumpleCondicion(reg)) {
            size_t pos = reg.find(valor);
            if (pos != string::npos) {
                reg.replace(pos, valor.length(), nuevoValor);
                cout << "Registro actualizado: " << reg << endl;
            }
        }
    }
}
    */