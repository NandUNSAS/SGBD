#include "../include/bufferManager.h"
#include <iostream>
#include <limits>

using namespace std;

// Constructor
bufferManager::bufferManager(int frames) : capacity(frames), time_counter(0) {
    for (int i = 0; i < capacity; i++) {
        this->frames.emplace_back(i);
    }
    cout << "BufferManager creado con " << frames << " frames" << endl;
}

// Agregar bloque al gestor de bloques (sin LRU)
void bufferManager::agregarGestorBloques(int id, bloque b) {
    gb.agregarBloque(id, b);
}

// Encontrar frame LRU disponible
int bufferManager::findFrameWithLowestPinCount() {
    int lowest_pin_index = -1;
    int min_pin_count = numeric_limits<int>::max();
    
    for (int i = 0; i < capacity; i++) {
        if (!frames[i].pinned && frames[i].pin_count < min_pin_count) {
            min_pin_count = frames[i].pin_count;
            lowest_pin_index = i;
        }
    }
    return lowest_pin_index;
}

int bufferManager::findLRUFrame() {
    int lru_index = -1;
    int min_time = numeric_limits<int>::max();
    int min_pin_count = numeric_limits<int>::max();
    
    // Primero intentamos encontrar el frame con menor tiempo de acceso
    for (int i = 0; i < capacity; i++) {
        if (!frames[i].pinned) {
            // Preferimos frames con menor pin_count
            if (frames[i].pin_count < min_pin_count || 
                (frames[i].pin_count == min_pin_count && frames[i].last_accessed < min_time)) {
                min_time = frames[i].last_accessed;
                min_pin_count = frames[i].pin_count;
                lru_index = i;
            }
        }
    }
    
    // Si no encontramos por LRU, buscamos el de menor pin_count
    if (lru_index == -1) {
        return findFrameWithLowestPinCount();
    }
    
    return lru_index;
}

// Escribir bloque a disco si está dirty
void bufferManager::writeToDisk(int block_id) {
    cout << "Escribiendo bloque " << block_id << " modificado a disco..." << endl;
    // Aquí iría la lógica real de escritura a disco
}

// Agregar bloque al buffer pool con política LRU
void bufferManager::agregarBufferPool(int id, bloque b, const string& mode) {
    time_counter++;
    
    // Buscar si el bloque ya está en buffer
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            frame.pin_count++;
            frame.dirty = (mode == "write");
            frame.pinned = (mode == "write");
            frame.last_accessed = time_counter;
            frame.mode = mode;
            
            cout << "Bloque " << id << " encontrado en frame " << frame.frame_id 
                 << " (PinCount: " << frame.pin_count << ")" << endl;
            return;
        }
    }
    
    // Buscar frame libre
    for (auto& frame : frames) {
        if (frame.block_id == -1) { // Frame libre
            frame.block_id = id;
            frame.pin_count = 1;
            frame.dirty = (mode == "write");
            frame.pinned = (mode == "write");
            frame.last_accessed = time_counter;
            frame.mode = mode;
            
            gb.agregarBloque(id, b);
            cout << "Cargado bloque " << id << " en frame libre " << frame.frame_id << endl;
            return;
        }
    }
    
    // Reemplazo LRU
    int lru_index = findLRUFrame();
    if (lru_index == -1) {
        cerr << "ERROR: No hay frames disponibles para reemplazo (todos están pinned)" << endl;
        return;
    }
    
    Frame& victim = frames[lru_index];
    cout << "Reemplazando bloque " << victim.block_id << " en frame " << victim.frame_id;
    
    if (victim.dirty) {
        cout << " (dirty - escribiendo a disco primero)";
        writeToDisk(victim.block_id);
    }
    cout << endl;
    
    // Reemplazar el bloque
    victim.block_id = id;
    victim.pin_count = 1;
    victim.dirty = (mode == "write");
    victim.pinned = (mode == "write");
    victim.last_accessed = time_counter;
    victim.mode = mode;
    
    gb.agregarBloque(id, b);
    cout << "Cargado bloque " << id << " en frame " << victim.frame_id << endl;
}

// Mostrar estado del buffer pool
void bufferManager::mostrarEstadoBufferPool() const {
    cout << "\n--- Estado del Buffer Pool ---" << endl;
    cout << "Frame | Block | PinCount | Dirty | Pinned | LastUsed | Mode" << endl;
    cout << "-----------------------------------------------------------" << endl;
    
    for (const auto& frame : frames) {
        cout << "  " << frame.frame_id << "   |  " 
             << (frame.block_id == -1 ? "-" : to_string(frame.block_id)) << "   |    "
             << frame.pin_count << "     |  "
             << (frame.dirty ? "Si" : "No") << "    |  "
             << (frame.pinned ? "Si" : "No") << "    |    "
             << frame.last_accessed << "    |  "
             << frame.mode << endl;
    }
}

// Acceder a un bloque existente
void bufferManager::accederBloque(int id, const string& mode) {
    time_counter++;
    
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            frame.pin_count++;
            frame.dirty = frame.dirty || (mode == "write");
            frame.pinned = frame.pinned || (mode == "write");
            frame.last_accessed = time_counter;
            frame.mode = mode;
            
            cout << "Acceso a bloque " << id << " en frame " << frame.frame_id 
                 << " (PinCount: " << frame.pin_count << ")" << endl;
            return;
        }
    }
    cerr << "Bloque " << id << " no encontrado en buffer pool" << endl;
}

// Despinear un bloque
void bufferManager::unpinBlock(int id) {
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            if (frame.pinned == true) {
                frame.pinned = false;
                
                if (frame.pinned == false) {
                    cout << "Bloque " << id << " despineado (ahora puede ser reemplazado)";
                    
                    if (frame.dirty) {
                        cout << " - Bloque marcado como dirty";
                    }
                    cout << endl;
                }
                return;
            }
        }
    }
    cerr << "Bloque " << id << " no encontrado en buffer pool" << endl;
}

void bufferManager::pinBlock(int id) {
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            if (frame.pinned == false) {
                frame.pinned = true;
                
                if (frame.pinned == true) {
                    cout << "Bloque " << id << " pineado";
                }
                return;
            }
        }
    }
    cerr << "Bloque " << id << " no encontrado en buffer pool" << endl;
}
// Verificar si un bloque está en buffer
bool bufferManager::bloqueEnBuffer(int id) const {
    for (const auto& frame : frames) {
        if (frame.block_id == id) {
            return true;
        }
    }
    return false;
}