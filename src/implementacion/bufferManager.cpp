#include "../include/bufferManager.h"
#include <iostream>
#include <limits>

using namespace std;

// Constructor
bufferManager::bufferManager(int frames, ReplacementPolicy policy) 
    : capacity(frames), time_counter(0), clock_hand(0), current_policy(policy) {
    for (int i = 0; i < capacity; i++) {
        this->frames.emplace_back(i);
    }
    cout << "BufferManager creado con " << frames << " frames y política ";
    cout << (current_policy == ReplacementPolicy::LRU ? "LRU" : "CLOCK") << endl;
}

void bufferManager::setReplacementPolicy(ReplacementPolicy policy) {
    current_policy = policy;
    cout << "Política de reemplazo cambiada a ";
    cout << (current_policy == ReplacementPolicy::LRU ? "LRU" : "CLOCK") << endl;
}

// Métodos auxiliares para LRU
int bufferManager::findLRUFrame() {
    int lru_index = -1;
    int min_time = numeric_limits<int>::max();
    int min_pin_count = numeric_limits<int>::max();
    
    for (int i = 0; i < capacity; i++) {
        if (!frames[i].pinned) {
            if (frames[i].pin_count < min_pin_count || 
                (frames[i].pin_count == min_pin_count && frames[i].last_accessed < min_time)) {
                min_time = frames[i].last_accessed;
                min_pin_count = frames[i].pin_count;
                lru_index = i;
            }
        }
    }
    
    if (lru_index == -1) {
        // Si todos están pinned, buscamos el de menor pin_count
        for (int i = 0; i < capacity; i++) {
            if (frames[i].pin_count < min_pin_count) {
                min_pin_count = frames[i].pin_count;
                lru_index = i;
            }
        }
    }
    
    return lru_index;
}

// Métodos auxiliares para CLOCK
int bufferManager::findClockFrame() {
    int attempts = 0;
    while (attempts < 2 * capacity) { // Para evitar loops infinitos
        Frame& frame = frames[clock_hand];
        
        if (!frame.pinned) {
            if (!frame.reference_bit) {
                int selected = clock_hand;
                clock_hand = (clock_hand + 1) % capacity;
                return selected;
            }
            frame.reference_bit = false;
        }
        
        clock_hand = (clock_hand + 1) % capacity;
        attempts++;
    }
    
    cerr << "ERROR: No se pudo encontrar frame para reemplazo (todos pinned)" << endl;
    return -1;
}

// Escribir bloque a disco si está dirty
void bufferManager::writeToDisk(int block_id) {
    cout << "Escribiendo bloque " << block_id << " modificado a disco..." << endl;
    // Aquí iría la lógica real de escritura a disco
}

// Agregar bloque al buffer pool
void bufferManager::agregarBufferPool(int id, bloque b, const string& mode, bool pinned) {
    time_counter++;
    
    // Buscar si el bloque ya está en buffer
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            frame.pin_count++;
            frame.dirty = frame.dirty || (mode == "write");
            frame.pinned = frame.pinned || pinned;  // Mantiene el pin si ya estaba
            frame.last_accessed = time_counter;
            frame.reference_bit = true;
            frame.mode = mode;
            
            cout << "Bloque " << id << " encontrado en frame " << frame.frame_id 
                 << " (PinCount: " << frame.pin_count << ")" << endl;
            return;
        }
    }
    
    // Buscar frame libre
    for (auto& frame : frames) {
        if (frame.block_id == -1) {
            frame.block_id = id;
            frame.pin_count = 1;
            frame.dirty = (mode == "write");
            frame.pinned = pinned;  // Usa el valor proporcionado por el usuario
            frame.last_accessed = time_counter;
            frame.reference_bit = true;
            frame.mode = mode;
            
            gb.agregarBloque(id, b);
            cout << "Cargado bloque " << id << " en frame libre " << frame.frame_id << endl;
            return;
        }
    }
    
    // Reemplazo según política seleccionada
    int replace_index = (current_policy == ReplacementPolicy::LRU) ? findLRUFrame() : findClockFrame();
    if (replace_index == -1) {
        cerr << "ERROR: No hay frames disponibles para reemplazo" << endl;
        return;
    }
    
    Frame& victim = frames[replace_index];
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
    victim.pinned = pinned;  // Usa el valor proporcionado por el usuario
    victim.last_accessed = time_counter;
    victim.reference_bit = true;
    victim.mode = mode;
    
    gb.agregarBloque(id, b);
    cout << "Cargado bloque " << id << " en frame " << victim.frame_id << endl;
}

// Mostrar estado del buffer pool
void bufferManager::mostrarEstadoBufferPool() const {
    cout << "\n--- Estado del Buffer Pool (" 
         << (current_policy == ReplacementPolicy::LRU ? "LRU" : "CLOCK") << ") ---" << endl;
         
    if (current_policy == ReplacementPolicy::CLOCK) {
        cout << "Clock hand posición: " << clock_hand << endl;
    }
    
    cout << "Frame | Block | PinCount | Dirty | Pinned | ";
    if (current_policy == ReplacementPolicy::LRU) {
        cout << "LastUsed | ";
    } else {
        cout << "RefBit | ";
    }
    cout << "Mode" << endl;
    cout << "-----------------------------------------------------------" << endl;
    
    for (const auto& frame : frames) {
        cout << "  " << frame.frame_id << "   |  " 
             << (frame.block_id == -1 ? "-" : to_string(frame.block_id)) << "   |    "
             << frame.pin_count << "     |  "
             << (frame.dirty ? "Si" : "No") << "    |  "
             << (frame.pinned ? "Si" : "No") << "    |    ";
             
        if (current_policy == ReplacementPolicy::LRU) {
            cout << frame.last_accessed << "    |  ";
        } else {
            cout << (frame.reference_bit ? "1" : "0") << "     |  ";
        }
        
        cout << frame.mode << endl;
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
            frame.reference_bit = true;
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
            if (frame.pinned) {
                frame.pinned = false;
                cout << "Bloque " << id << " despineado (ahora puede ser reemplazado)";
                if (frame.dirty) {
                    cout << " - Bloque marcado como dirty";
                }
                cout << endl;
            } else {
                cout << "Bloque " << id << " ya estaba despineado" << endl;
            }
            return;
        }
    }
    cerr << "Bloque " << id << " no encontrado en buffer pool" << endl;
}

// Pinear un bloque
void bufferManager::pinBlock(int id) {
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            if (!frame.pinned) {
                frame.pinned = true;
                cout << "Bloque " << id << " pineado (no podrá ser reemplazado)" << endl;
            } else {
                cout << "Bloque " << id << " ya estaba pineado" << endl;
            }
            return;
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

// Agregar bloque al gestor de bloques (sin LRU/CLOCK)
void bufferManager::agregarGestorBloques(int id, bloque b) {
    gb.agregarBloque(id, b);
}