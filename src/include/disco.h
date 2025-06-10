#ifndef DISCO_H
#define DISCO_H

class disco {
    private:
        int platos;
        int superficies;
        int pistasXsuperficie;
        int sectoresXpista;
        int tamSector;
    

        int capacidadDisco;
     
            
    public:
        disco();
     
       void discoEstatico();
        void discoPersonalizado(int _platos, int _pistasXplato, int _sectorXpista, int _tamSector);
        //void setDisco();
        void crearDisco();

        void calcularCapacidadDisco();
        void guardarEnArchivo();

        void setPlatos(int _platos);
        void setPistasXsuperficie(int _pistasXsuperficie);
        void setSectoresXplato(int _sectoresXPlato);
        void setTamSecto(int sectoresXbloque);
        void setCapacidadDisco(int _capacidadDisco);

        int getPlatos();
        int getSuperficie();
        int getPistasXsuperficie();
        int getSectoresXpista();
        int getTamSector();
        int getCapacidadDisco();

};

#endif // DISCO_H