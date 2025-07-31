#include "../include/hashing_extendible.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

ExtendibleHash::ExtendibleHash(int depth, int size, const std::string& filename) 
    : global_depth(depth), bucket_size(size), index_file(filename) {
    int size_dir = 1 << global_depth;
    directory.resize(size_dir, 0);
    buckets.emplace_back(); // Bucket 0
}

int ExtendibleHash::hash(int key) const {
    return key & ((1 << global_depth) - 1);
}

int ExtendibleHash::pairIndex(int bucket_idx, int depth) const {
    return bucket_idx ^ (1 << (depth - 1));
}

void ExtendibleHash::grow() {
    global_depth++;
    directory.resize(1 << global_depth);
    
    // Duplicar los punteros del directorio
    for(int i = (1 << (global_depth - 1)); i < (1 << global_depth); i++) {
        directory[i] = directory[i - (1 << (global_depth - 1))];
    }
}

void ExtendibleHash::shrink() {
    bool can_shrink = true;
    
    for(int i = 0; i < directory.size(); i++) {
        if(buckets[directory[i]].size() > 0 && 
           buckets[directory[i]].begin()->second.local_depth == global_depth) {
            can_shrink = false;
            break;
        }
    }
    
    if(can_shrink) {
        global_depth--;
        directory.resize(1 << global_depth);
    }
}

void ExtendibleHash::split(int bucket_idx) {
    auto& bucket = buckets[directory[bucket_idx]];
    int local_depth = bucket.begin()->second.local_depth;
    
    if(local_depth == global_depth) {
        grow();
    }
    
    // Crear nuevo bucket
    int new_bucket_idx = buckets.size();
    buckets.emplace_back();
    
    // Actualizar profundidades
    for(auto& entry : bucket) {
        entry.second.local_depth = local_depth + 1;
    }
    for(auto& entry : buckets[new_bucket_idx]) {
        entry.second.local_depth = local_depth + 1;
    }
    
    // Reubicar entradas
    int pair_idx = pairIndex(bucket_idx, local_depth + 1);
    directory[pair_idx] = new_bucket_idx;
    
    // Reasignar claves
    std::vector<int> keys_to_remove;
    for(const auto& entry : bucket) {
        if(hash(entry.first) == pair_idx) {
            buckets[new_bucket_idx][entry.first] = entry.second;
            keys_to_remove.push_back(entry.first);
        }
    }
    
    for(int key : keys_to_remove) {
        bucket.erase(key);
    }
}

void ExtendibleHash::merge(int bucket_idx) {
    int local_depth = buckets[directory[bucket_idx]].begin()->second.local_depth;
    int pair_idx = pairIndex(bucket_idx, local_depth);
    
    if(buckets[directory[pair_idx]].size() == 0 && 
       buckets[directory[pair_idx]].begin()->second.local_depth == local_depth) {
        
        // Fusionar buckets
        for(auto& entry : buckets[directory[pair_idx]]) {
            buckets[directory[bucket_idx]][entry.first] = entry.second;
        }
        
        // Actualizar directorio
        for(int i = 0; i < directory.size(); i++) {
            if(directory[i] == directory[pair_idx]) {
                directory[i] = directory[bucket_idx];
            }
        }
        
        // Eliminar bucket vacío
        buckets[directory[pair_idx]].clear();
        shrink();
    }
}

void ExtendibleHash::insert(int key, const BlockPosition& position) {
    int bucket_idx = hash(key);
    auto& bucket = buckets[directory[bucket_idx]];
    
    // Si la clave ya existe, actualizar posición
    if(bucket.find(key) != bucket.end()) {
        bucket[key] = position;
        return;
    }
    
    // Si el bucket está lleno, hacer split
    if(bucket.size() >= bucket_size) {
        split(bucket_idx);
        insert(key, position); // Reintentar inserción
        return;
    }
    
    // Insertar nueva entrada
    BlockPosition pos_with_depth = position;
    pos_with_depth.local_depth = buckets[directory[bucket_idx]].begin()->second.local_depth;
    bucket[key] = pos_with_depth;
}

BlockPosition ExtendibleHash::search(int key) const {
    int bucket_idx = hash(key);
    const auto& bucket = buckets[directory[bucket_idx]];
    
    auto it = bucket.find(key);
    if(it != bucket.end()) {
        return it->second;
    }
    
    return BlockPosition(); // Retorna posición inválida si no se encuentra
}

bool ExtendibleHash::remove(int key) {
    int bucket_idx = hash(key);
    auto& bucket = buckets[directory[bucket_idx]];
    
    if(bucket.erase(key) > 0) {
        merge(bucket_idx);
        return true;
    }
    
    return false;
}

void ExtendibleHash::saveToFile() const {
    std::ofstream out(index_file, std::ios::binary);
    if(!out) return;
    
    // Escribir metadatos
    out.write(reinterpret_cast<const char*>(&global_depth), sizeof(global_depth));
    out.write(reinterpret_cast<const char*>(&bucket_size), sizeof(bucket_size));
    
    // Escribir directorio
    int dir_size = directory.size();
    out.write(reinterpret_cast<const char*>(&dir_size), sizeof(dir_size));
    out.write(reinterpret_cast<const char*>(directory.data()), dir_size * sizeof(int));
    
    // Escribir buckets
    int num_buckets = buckets.size();
    out.write(reinterpret_cast<const char*>(&num_buckets), sizeof(num_buckets));
    
    for(const auto& bucket : buckets) {
        int bucket_size = bucket.size();
        out.write(reinterpret_cast<const char*>(&bucket_size), sizeof(bucket_size));
        
        for(const auto& entry : bucket) {
            out.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
            out.write(reinterpret_cast<const char*>(&entry.second.block_id), sizeof(entry.second.block_id));
            out.write(reinterpret_cast<const char*>(&entry.second.offset), sizeof(entry.second.offset));
            out.write(reinterpret_cast<const char*>(&entry.second.local_depth), sizeof(entry.second.local_depth));
        }
    }
}

bool ExtendibleHash::loadFromFile() {
    std::ifstream in(index_file, std::ios::binary);
    if(!in) return false;
    
    // Leer metadatos
    in.read(reinterpret_cast<char*>(&global_depth), sizeof(global_depth));
    in.read(reinterpret_cast<char*>(&bucket_size), sizeof(bucket_size));
    
    // Leer directorio
    int dir_size;
    in.read(reinterpret_cast<char*>(&dir_size), sizeof(dir_size));
    directory.resize(dir_size);
    in.read(reinterpret_cast<char*>(directory.data()), dir_size * sizeof(int));
    
    // Leer buckets
    int num_buckets;
    in.read(reinterpret_cast<char*>(&num_buckets), sizeof(num_buckets));
    buckets.resize(num_buckets);
    
    for(auto& bucket : buckets) {
        int bucket_entries;
        in.read(reinterpret_cast<char*>(&bucket_entries), sizeof(bucket_entries));
        
        for(int i = 0; i < bucket_entries; i++) {
            int key;
            BlockPosition pos;
            in.read(reinterpret_cast<char*>(&key), sizeof(key));
            in.read(reinterpret_cast<char*>(&pos.block_id), sizeof(pos.block_id));
            in.read(reinterpret_cast<char*>(&pos.offset), sizeof(pos.offset));
            in.read(reinterpret_cast<char*>(&pos.local_depth), sizeof(pos.local_depth));
            
            bucket[key] = pos;
        }
    }
    
    return true;
}

void ExtendibleHash::display() const {
    std::cout << "Global Depth: " << global_depth << "\n";
    std::cout << "Directory:\n";
    
    for(int i = 0; i < directory.size(); i++) {
        std::cout << std::bitset<8>(i).to_string().substr(8 - global_depth) 
                  << " -> Bucket " << directory[i] << "\n";
    }
    
    std::cout << "\nBuckets:\n";
    for(int i = 0; i < buckets.size(); i++) {
        if(!buckets[i].empty()) {
            std::cout << "Bucket " << i << " (Local Depth: " 
                      << buckets[i].begin()->second.local_depth << "):\n";
            
            for(const auto& entry : buckets[i]) {
                std::cout << "  " << entry.first << " -> Bloque " << entry.second.block_id 
                          << ", Offset " << entry.second.offset << "\n";
            }
        }
    }
}