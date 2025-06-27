#ifndef BLOQUE_H
#define BLOQUE_H
#include <string>

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
//
};

#endif // !BLOQUE_H
