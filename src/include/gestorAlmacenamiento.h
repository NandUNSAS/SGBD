#ifndef GESTORALMACENAMIENTO_H
#define GESTORALMACENAMIENTO_H
#include "controladorDisco.h"
#include "disco.h"
#include <string>

using namespace std;

class gestorAlmacenamiento{
    private:
    disco discoDuro;
    controladorDisco cd;
    
    int sectoresXbloque;
    int capacidadBloque;

    int bloquesXpista;
    int bloquesXplato;

    int bloquesTotales;

    int longitudRegistroMax;

    public:
        gestorAlmacenamiento(controladorDisco& controlador) : cd(controlador) {}
        void setDisco(const disco& _discoDuro);
        void calcularCapacidadBloque();
        void setSectoresXbloque(int _sectoresXbloque);
        void setLongitudRegistroMax(int _longitudRegistroMax);
        
        int getCapacidadBloque();
        int getBloquesXpista();
        int getBloquesXplato();
        int getBloquesTotales();

        void calcularBloquesXpista();
        void calcularBloquesXplato();
        void calcularBloquesTotales();

        void generarArchivoRutas();

        void insertarRutasBloques(const string& rutaReservadoBloques, const string& rutaSectores);
       
        void definirBloques();
        void establecerHeaders(const string& rutaSector, string rutaPlantillaHeader);
        void insertarRegistro(string registro, string rutaSector, int tamanoRegistro);
        void insertarRegistroB(string registro, string rutaSector, int tamanoRegistro);
        void guardarEnArchivo();

        int capacidadDisco();
    };

#endif // !GESTOR_ALMACENAMIENTO_H
