#ifndef FUNCIONESBLOQUE_H
#define FUNCIONESBLOQUE_H
#include <string>
#include <vector>
using namespace std;

vector<string> obtenerCampos(const string& linea);
vector<int> obtenerCamposInt(const string& linea);
vector<int> convertirVectorAEnteros(const vector<string>& strVector);

void imprimirVectorString(const vector<string>& vec);
void imprimirVectorInt(const vector<int>& vec);

int devolverCampo(const string& contenido_sector, int campo_n);
bool actualizarCabecera(int tamanoRegistro, string ruta_sector, int capacidadSector);
bool actualizarCabeceraB(int tamanoRegistro, string ruta_sector, int capacidadSector, int sectoresXbloque);

#endif // !FUNCIONESBLOQUE_H
