#ifndef DIRECTORIO_HASH_H
#define DIRECTORIO_HASH_H

#include <vector>
#include <map>
#include <string>

class Bucket {
private:
    int depth;
    int size;
    std::map<int, int> values; // key: id_postulante, value: id_bloque
public:
    Bucket(int depth, int size);
    int insert(int key, int bloque_id);
    int search(int key);
    bool isFull();
    int getDepth();
    int increaseDepth();
    std::map<int, int> copy();
    void clear();
};

class DirectorioHash {
private:
    int global_depth;
    int bucket_size;
    std::vector<Bucket*> buckets;
    
    int hash(int n);
    void split(int bucket_no);
    void grow();

public:
    DirectorioHash(int depth = 2, int bucket_size = 4);
    void insert(int id_postulante, int id_bloque);
    int search(int id_postulante);
    void cargarDesdeArchivo(const std::string& rutaArchivo);
    void mostrarTablaHash() const;
};

#endif