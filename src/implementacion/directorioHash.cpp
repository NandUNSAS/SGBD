#include "../include/directorioHash.h"
#include <fstream>
#include <sstream>
#include <iostream>

#define MAX_ITERACIONES 5000

Bucket::Bucket(int depth, int size) : depth(depth), size(size) {}

int Bucket::insert(int key, int bloque_id) {
    if (isFull()) return 0;
    values[key] = bloque_id;
    return 1;
}

int Bucket::search(int key) {
    auto it = values.find(key);
    if (it != values.end()) return it->second;
    return -1;
}

bool Bucket::isFull() {
    return values.size() >= size;
}

int Bucket::getDepth() { return depth; }
int Bucket::increaseDepth() { return ++depth; }
std::map<int, int> Bucket::copy() { return values; }
void Bucket::clear() { values.clear(); }

DirectorioHash::DirectorioHash(int depth, int bucket_size)
    : global_depth(depth), bucket_size(bucket_size) {
    buckets.resize(1 << global_depth);
    for (int i = 0; i < (1 << global_depth); i++) {
        buckets[i] = new Bucket(global_depth, bucket_size);
    }
}

int DirectorioHash::hash(int n) {
    return n & ((1 << global_depth) - 1);
}

void DirectorioHash::grow() {
    int old_size = buckets.size();
    buckets.resize(old_size * 2);
    for (int i = 0; i < old_size; i++) {
        buckets[i + old_size] = buckets[i];
    }
    global_depth++;
    std::cout << "[DEBUG] grow() llamado. Nuevo global_depth: " << global_depth << ", buckets: " << buckets.size() << std::endl;
}

void DirectorioHash::split(int bucket_no) {
    int local_depth = buckets[bucket_no]->increaseDepth();
    if (local_depth > global_depth) {
        grow();
    }
    int pair_index = bucket_no ^ (1 << (local_depth - 1));
    // Solo crear un nuevo bucket si el par apunta al mismo bucket
    if (buckets[pair_index] == buckets[bucket_no]) {
        buckets[pair_index] = new Bucket(local_depth, bucket_size);
        std::cout << "[DEBUG] split() - Nuevo bucket creado en pair_index: " << pair_index << std::endl;
    }
    // Redistribuir los elementos
    auto temp = buckets[bucket_no]->copy();
    buckets[bucket_no]->clear();

    int contador = 0;
    for (const auto& pair : temp) {
        insert(pair.first, pair.second);
        contador++;
        if (contador > MAX_ITERACIONES) {
            std::cerr << "[ERROR] split() - Se alcanzó el límite de iteraciones (" << MAX_ITERACIONES << ")." << std::endl;
            break;
        }
    }
    std::cout << "[DEBUG] split() - Redistribuidos " << contador << " elementos." << std::endl;
}

void DirectorioHash::insert(int id_postulante, int id_bloque) {
    int bucket_no = hash(id_postulante);
    int status = buckets[bucket_no]->insert(id_postulante, id_bloque);

    int contador = 0;
    while (status == 0) {
        split(bucket_no);
        if (buckets[hash(id_postulante)]->isFull()) {
            bucket_size++;
            std::cout << "[DEBUG] insert() - bucket_size aumentado a " << bucket_size << std::endl;
            buckets[hash(id_postulante)]->insert(id_postulante, id_bloque);
            break;
        } else {
            status = buckets[hash(id_postulante)]->insert(id_postulante, id_bloque);
        }
        contador++;
        if (contador > MAX_ITERACIONES) {
            std::cerr << "[ERROR] insert() - Se alcanzó el límite de iteraciones (" << MAX_ITERACIONES << ")." << std::endl;
            break;
        }
    }
    //mostrarTablaHash(); // <-- Mostrar la tabla hash después de cada inserción
}

int DirectorioHash::search(int id_postulante) {
    int bucket_no = hash(id_postulante);
    return buckets[bucket_no]->search(id_postulante);
}

void DirectorioHash::cargarDesdeArchivo(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    std::string linea;
    int contador = 0;
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string id_str, bloque_str;
        std::getline(ss, id_str, '#');
        std::getline(ss, bloque_str);
        if (id_str.empty() || bloque_str.empty()) continue;
        int id_postulante = std::stoi(id_str);
        int id_bloque = std::stoi(bloque_str);
        insert(id_postulante, id_bloque);
        contador++;
       // mostrarTablaHash(); // <-- Mostrar la tabla hash en cada iteración
        if (contador > MAX_ITERACIONES) {
            std::cerr << "[ERROR] cargarDesdeArchivo() - Se alcanzó el límite de iteraciones (" << MAX_ITERACIONES << ")." << std::endl;
            break;
        }
    }
    std::cout << "[DEBUG] cargarDesdeArchivo() - Cargados " << contador << " registros." << std::endl;
}

void DirectorioHash::mostrarTablaHash() const {
    std::ofstream out("../../tabla_hash.txt");
    out << "\n--- Tabla Hash Extendible ---\n";
    for (size_t i = 0; i < buckets.size(); ++i) {
        out << "Bucket " << i << " (depth=" << buckets[i]->getDepth() << "): ";
        for (const auto& par : buckets[i]->copy()) {
            out << "[" << par.first << "->" << par.second << "] ";
        }
        out << std::endl;
    }
    out << "Global depth: " << global_depth << std::endl;
    out.close();
}