#ifndef HASHING_EXTENDIBLE_H
#define HASHING_EXTENDIBLE_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <bitset>

struct BlockPosition {
    int block_id;
    int offset;
    int local_depth; // Profundidad local del bucket
    
    BlockPosition() : block_id(-1), offset(-1), local_depth(0) {}
    BlockPosition(int b, int o, int ld = 0) : block_id(b), offset(o), local_depth(ld) {}
    
    bool isValid() const { return block_id != -1; }
};

class ExtendibleHash { 
private:
    int global_depth; // Profundidad global del directorio
    int bucket_size; // Tamaño máximo de cada bucket
    std::string index_file; // Nombre del archivo donde se guarda el índice
    std::vector<int> directory; // Directorio de punteros a buckets
    std::vector<std::map<int, BlockPosition>> buckets; // Buckets que almacenan pares clave-posición
    
    void split(int bucket_idx); // Divide un bucket cuando se llena
    void merge(int bucket_idx); 
    int hash(int key) const;
    int pairIndex(int bucket_idx, int depth) const; // Calcula el índice del par de bits para el bucket dado
    void grow();
    void shrink();
    
public:
    ExtendibleHash(int depth = 1, int size = 4, const std::string& filename = "indice_postulantes.txt");
    
    void insert(int key, const BlockPosition& position); // Inserta un nuevo par clave-posición
    BlockPosition search(int key) const; // Busca la posición de un bloque dado su clave
    bool remove(int key); // Elimina un par clave-posición
    
    void saveToFile() const; // Guarda el índice en un archivo
    bool loadFromFile(); // Carga el índice desde un archivo
    
    void display() const; // Muestra el estado actual del índice
};

#endif