#ifndef ESQUEMA_H
#define ESQUEMA_H

#include <string>
#include <vector>

using namespace std;

struct CampoEsquema {
    string nombre;
    string tipo;
    int tamano;
    int posicion; // Posición del campo (0-based)
};

class Esquema {
private:
    string nombreTabla;
    vector<CampoEsquema> campos;

public:
    // Constructor que procesa el esquema desde un archivo
    Esquema() = default;
    // Establece el nombre de la tabla/relación
    void setNombreTabla(const string& nombre);
    
    void construirEsquema(const string& archivoEsquema);
    // Obtiene el nombre de la tabla/relación
    string getNombreTabla() const;
    
    // Busca la posición de un campo por nombre
    int buscarPosicionCampo(const string& nombreCampo) const;
    
    // Busca las posiciones de múltiples campos
    vector<int> buscarPosicionesCampos(const vector<string>& nombresCampos) const;
    
    // Muestra el esquema por pantalla
    void mostrarEsquema() const;
    
    // Genera entrada para archivo de indexación
    
    // Obtiene todos los campos del esquema
    const vector<CampoEsquema>& getCampos() const;
    
};

#endif // ESQUEMA_H