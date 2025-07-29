#include "../include/query.h"
#include <iostream>
#include <limits>
#include "../include/esquema.h"
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
    // Limpiar el buffer de entrada
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Introduce el nombre de la tabla: ";
    getline(cin, tabla);
    esquema.construirEsquema(tabla);

    if (tipo == "SELECT") {
        pedirCamposSelect();
        pedirWhereSelect();
    }
    else if (tipo == "DELETE") {
        pedirWhereBasico();
    }
    else if (tipo == "UPDATE") {
        pedirWhereBasico();
        pedirDatosUpdate();
    }
    else if (tipo == "INSERT") {
        pedirDatosInsert();
    }

    imprimirConsulta();
}

void Query::pedirCamposSelect() {
    int opcion = 0;
    while (opcion != 1 && opcion != 2) {
        cout << "\nSelección de campos:\n";
        cout << "1. Todos los campos (*)\n";
        cout << "2. Campos específicos\n";
        cout << "Opción: ";
        cin >> opcion;
        cin.ignore();
        if (opcion != 1 && opcion != 2)
            cout << "Opción inválida, inténtalo de nuevo.\n";
    }

    if (opcion == 1){
        todos_campos = true;
        camposSelect.clear();
    }
    if (opcion == 2) {
        todos_campos = false;
        int n;
        cout << "¿Cuántos campos deseas seleccionar?: ";
        cin >> n;
        cin.ignore();
        
        camposSelect.resize(n);
        for (int i = 0; i < n; ++i) {
            cout << "Nombre del campo #" << (i + 1) << ": ";
            getline(cin, camposSelect[i]);
        }
    }
}

void Query::pedirWhereSelect() {
    cout << "\n¿Deseas aplicar cláusula WHERE? (1: Sí, 0: No): ";
    int opcion;
    cin >> opcion;
    cin.ignore();
    tieneWhere = (opcion == 1);

    if (tieneWhere) {
        cout << "\nTipo de condición WHERE:\n";
        cout << "1. Igualdad (=)\n";
        cout << "2. Rango (BETWEEN)\n";
        cout << "3. Mayor que (>)\n";
        cout << "4. Menor que (<)\n";
        cout << "5. Mayor o igual que (>=)\n";
        cout << "6. Menor o igual que (<=)\n";
        cout << "Opción: ";
        cin >> opcionCondicion;
        cin.ignore();

        cout << "Introduce el nombre del atributo: ";
        getline(cin, atributo);

        switch(opcionCondicion) {
            case 1: // Igualdad
                cout << "Introduce el valor para comparar (=): ";
                getline(cin, valor);
                break;
            case 2: // Rango
                cout << "Introduce el valor mínimo (BETWEEN X AND Y): ";
                getline(cin, valorMinimo);
                cout << "Introduce el valor máximo (BETWEEN X AND Y): ";
                getline(cin, valorMaximo);
                break;
            case 3: // Mayor que
                cout << "Introduce el valor mínimo (>): ";
                getline(cin, valorMinimo);
                break;
            case 4: // Menor que
                cout << "Introduce el valor máximo (<): ";
                getline(cin, valorMaximo);
                break;
            case 5: // Mayor o igual
                cout << "Introduce el valor mínimo (>=): ";
                getline(cin, valorMinimo);
                break;
            case 6: // Menor o igual
                cout << "Introduce el valor máximo (<=): ";
                getline(cin, valorMaximo);
                break;
        }
    }
}

void Query::pedirWhereBasico() {
    cout << "¿Deseas aplicar cláusula WHERE? (1: Sí, 0: No): ";
    int opcion;
    cin >> opcion;
    cin.ignore();
    tieneWhere = (opcion == 1);

    if (tieneWhere) {
        cout << "Introduce el nombre del atributo: ";
        getline(cin, atributo);
        cout << "Introduce el valor para comparar: ";
        getline(cin, valor);
    }
}

void Query::pedirDatosUpdate() {
    cout << "Introduce el nombre del atributo a modificar: ";
    getline(cin, atributoUpdate);
    cout << "Introduce el nuevo valor: ";
    getline(cin, nuevoValor);
}

void Query::pedirDatosInsert() {
    // Obtener todos los campos del esquema
    const vector<CampoEsquema>& campos = esquema.getCampos();
    
    // Mostrar los campos disponibles
    cout << "\nCampos disponibles para la tabla '" << esquema.getNombreTabla() << "':" << endl;
    for (const auto& campo : campos) {
        cout << "- " << campo.nombre << " (" << campo.tipo;
        if (campo.tamano > 0) cout << ", tamaño: " << campo.tamano;
        cout << ")" << endl;
    }

    // Preparar vectores para almacenar los datos
    columnas.clear();
    valores.clear();
    
    cout << "\nIngrese los valores para cada campo (deje vacío para omitir):" << endl;
    
    for (const auto& campo : campos) {
        string valor;
        cout << campo.nombre << " (" << campo.tipo << "): ";
        getline(cin, valor);
        
        if (!valor.empty()) {
            columnas.push_back(campo.nombre);
            valores.push_back(valor);
        }
    }

    // Verificar que se ingresó al menos un valor
    if (columnas.empty()) {
        cout << "No se ingresaron valores. Operación cancelada." << endl;
        columnas.clear();
        valores.clear();
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

int Query::getOpcionCondicion() const{
    return opcionCondicion;
}

bool Query::getTodos_Campos() const {
    return todos_campos;
}

Esquema Query::getEsquema(){
    return esquema;
}

vector<string> Query::getValores(){
    return valores;
}

    void Query::setEsquema(Esquema& _esquema){
    esquema = _esquema;
}

string Query::generarConsultaSQL() const {
    string consulta;
    
    if (tipo == "SELECT") {
        consulta = "SELECT ";
        
        // Campos a seleccionar
        if (camposSelect.empty()) {
            consulta += "*";
        } else {
            for (size_t i = 0; i < camposSelect.size(); ++i) {
                if (i != 0) consulta += ", ";
                consulta += camposSelect[i];
            }
        }
        
        consulta += " FROM " + tabla;
        
        // Cláusula WHERE
        if (tieneWhere) {
            consulta += " WHERE " + atributo;
            
            switch(opcionCondicion) {
                case 1: // Igualdad
                    consulta += " = '" + valor + "'";
                    break;
                case 2: // BETWEEN
                    consulta += " BETWEEN '" + valorMinimo + "' AND '" + valorMaximo + "'";
                    break;
                case 3: // Mayor que
                    consulta += " > '" + valorMinimo + "'";
                    break;
                case 4: // Menor que
                    consulta += " < '" + valorMaximo + "'";
                    break;
                case 5: // Mayor o igual
                    consulta += " >= '" + valorMinimo + "'";
                    break;
                case 6: // Menor o igual
                    consulta += " <= '" + valorMaximo + "'";
                    break;
                case 7: // LIKE
                    consulta += " LIKE '" + valor + "'";
                    break;
            }
        }
    }
    else if (tipo == "INSERT") {
        consulta = "INSERT INTO " + tabla + " (";
        
        // Columnas
        for (size_t i = 0; i < columnas.size(); ++i) {
            if (i != 0) consulta += ", ";
            consulta += columnas[i];
        }
        
        consulta += ") VALUES (";
        
        // Valores
        for (size_t i = 0; i < valores.size(); ++i) {
            if (i != 0) consulta += ", ";
            consulta += "'" + valores[i] + "'";
        }
        
        consulta += ")";
    }
    else if (tipo == "DELETE") {
        consulta = "DELETE FROM " + tabla;
        
        if (tieneWhere) {
            consulta += " WHERE " + atributo + " = '" + valor + "'";
        }
    }
    else if (tipo == "UPDATE") {
        consulta = "UPDATE " + tabla + " SET " + atributoUpdate + " = '" + nuevoValor + "'";
        
        if (tieneWhere) {
            consulta += " WHERE " + atributo + " = '" + valor + "'";
        }
    }
    
    return consulta;
}

void Query::imprimirConsulta() const {
    cout << "\n=== CONSULTA SQL GENERADA ===\n";
    cout << generarConsultaSQL() << endl;
    cout << "============================\n";
}