#ifndef BLOQUE_H
#define BLOQUE_H
#include <string>
#include <iostream>

using namespace std;

class bloque{
    int idBloque;
    string rutaBloque; 
    string contenido;

    
    public:
    bloque();
    void inicializarBloque(int id, string ruta);
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
//
};

#endif // !BLOQUE_H
