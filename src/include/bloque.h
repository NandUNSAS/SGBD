#ifndef BLOQUE_H
#define BLOQUE_H
#include <string>
#include <iostream>
#include <vector>

using namespace std;

struct EntradaSector {
    int estado;         // 0 = libre, 1 = ocupado
    string rutaSector;
};

class bloque{
    int idBloque;
    string rutaBloque; 
    string contenido;
    vector<EntradaSector> rutaSectores;
    int sectoresXbloque;
    int idPostulante;

    public:
    bloque();
    void inicializarBloque(int id, string ruta);
    void inicializarBloqueInt(int id, string ruta, int _idPostulante);
    void construirBloque();
    void mostrarBloque()const;

    // Métodos set
    void setIdBloque(int id);
    void setRutaBloque(const string& ruta);

    // Métodos get
    int getIdBloque() const;
    string getRutaBloque() const;
    string getContenido() const;
    void escribirPaginaSimulada(const string& nuevosDatos) {
        cout << "Simulando escritura en el bloque " << idBloque << endl;
        cout << "Contenido anterior: " << contenido.substr(0, 50) << "..." << endl;
        contenido = nuevosDatos;
        cout << "Nuevo contenido: " << contenido.substr(0, 50) << "..." << endl;
    }

    void leerRutasSectoresDesdeArchivo();
    string buscarRegistroPorIdPostulante() const;

    int getIdPostulante() const;
//
};

#endif // !BLOQUE_H
