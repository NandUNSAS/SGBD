#include "../include/bufferManager.h"
#include "../include/bloque.h"
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
    while (attempts < 2 * capacity) { // Evitar loops infinitos
        Frame& frame = frames[clock_hand];
        
        if (!frame.pinned) {
            if (!frame.reference_bit) {
                // ¡Frame candidato encontrado!
                if (frame.dirty) {
                    // Paso 1: Escribir a disco si es dirty
                    writeToDisk(frame.block_id);
                    frame.dirty = false; // Paso 2: Resetear dirty bit
                }
                int selected = clock_hand;
                // Paso 3: Mover manecilla al siguiente frame
                clock_hand = (clock_hand + 1) % capacity;
                return selected; // Retorna el frame a reemplazar
            } else {
                // Paso 4: Si ref_bit=1, darle segunda oportunidad
                frame.reference_bit = false;
            }
        }
        
        // Mover manecilla
        clock_hand = (clock_hand + 1) % capacity;
        attempts++;
    }
    
    cerr << "ERROR: No se encontró frame para reemplazo (todos pinned)" << endl;
    return -1;
}

// Escribir bloque a disco si está dirty
void bufferManager::writeToDisk(int block_id) {
    cout << "SIMULACIÓN: Escribiendo bloque " << block_id 
         << " a disco (por dirty_bit=1)" << endl;
    // En un sistema real, aquí se escribirían los datos reales al disco
}

// Agregar bloque al buffer pool
// En bufferManager.cpp

void bufferManager::agregarBufferPool(int id, bloque b, const string& mode, bool pinned) {
    time_counter++;
    
    // Buscar si el bloque ya está en buffer
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            // Actualizar el frame
            frame.pin_count++;
            frame.dirty = frame.dirty || (mode == "write");
            frame.pinned = pinned;
            frame.last_accessed = time_counter;
            frame.reference_bit = true;
            frame.mode = mode;
            
            // Mostrar estado solo si es modo lectura
            if (mode == "read") {
                cout << "\n--- Estado después de carga (HIT) ---" << endl;
                mostrarEstadoBufferPool();
                
                // Mostrar contenido del bloque
                cout << "\nContenido del bloque " << id << ":" << endl;
                gb.mostrarBloque(id);
                
                // Decrementar sin mostrar tabla
                frame.pin_count--;
            }
            
            return;
        }
    }
    
    // Si no está en buffer, buscar frame libre
    for (auto& frame : frames) {
        if (frame.block_id == -1) {
            frame.block_id = id;
            frame.pin_count = 1;
            frame.dirty = (mode == "write");
            frame.pinned = pinned;
            frame.last_accessed = time_counter;
            frame.reference_bit = true;
            frame.mode = mode;
            
            gb.agregarBloque(id, b);
            
            // Mostrar estado solo si es modo lectura
            if (mode == "read") {
                cout << "\n--- Estado después de carga (MISS) ---" << endl;
                mostrarEstadoBufferPool();
                
                // Mostrar contenido del bloque
                cout << "\nContenido del bloque " << id << ":" << endl;
                gb.mostrarBloque(id);
                
                // Decrementar sin mostrar tabla
                frame.pin_count--;
            }
            
            return;
        }
    }
    
    // Si no hay frames libres, aplicar política de reemplazo
    int replace_index = (current_policy == ReplacementPolicy::LRU) ? findLRUFrame() : findClockFrame();
    if (replace_index == -1) {
        cerr << "ERROR: No hay frames disponibles para reemplazo" << endl;
        return;
    }
    
    Frame& victim = frames[replace_index];
    if (victim.dirty) {
        writeToDisk(victim.block_id);
    }
    
    // Reemplazar el bloque
    victim.block_id = id;
    victim.pin_count = 1;
    victim.dirty = (mode == "write");
    victim.pinned = pinned;
    victim.last_accessed = time_counter;
    victim.reference_bit = true;
    victim.mode = mode;
    
    gb.agregarBloque(id, b);
    
    // Mostrar estado solo si es modo lectura
    if (mode == "read") {
        cout << "\n--- Estado después de reemplazo ---" << endl;
        mostrarEstadoBufferPool();
        
        // Mostrar contenido del bloque
        cout << "\nContenido del bloque " << id << ":" << endl;
        gb.mostrarBloque(id);
        
        // Decrementar sin mostrar tabla
        victim.pin_count--;
    }
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
    time_counter++;  // Actualiza el "reloj" global del buffer

    // Busca el bloque en los frames del buffer
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            // 1. Actualiza metadatos
            frame.pin_count++;           // Bloque "en uso"
            frame.dirty = frame.dirty || (mode == "write"); // Marcar como modificado si es escritura
            frame.pinned = frame.pinned || (mode == "write"); // Pinear si es escritura
            frame.last_accessed = time_counter;  // Actualiza LRU
            frame.reference_bit = true;          // Actualiza CLOCK
            frame.mode = mode;                   // Guarda el tipo de acceso

            // 2. Comportamiento específico para lectura
            if (mode == "read") {
                cout << "\n--- Estado después de acceso ---" << endl;
                mostrarEstadoBufferPool();  // Muestra tabla con pin_count incrementado
                
                cout << "\nContenido del bloque " << id << ":" << endl;
                gb.mostrarBloque(id);       // Muestra el contenido
                
                frame.pin_count--;          // Decrementa silenciosamente (sin mostrar tabla)
            }

            return;
        }
    }
    // Si el bloque no está en buffer:
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

void bufferManager::mostrarContenidoBloque(int id) const {
    for (const auto& frame : frames) {
        if (frame.block_id == id) {
            if (frame.mode == "read") {
                cout << "\n--- Contenido del Bloque " << id << " ---" << endl;
                // Buscar el bloque en el gestor de bloques y mostrarlo
                gb.mostrarBloque(id);
                return;
            }
        }
    }
    cerr << "Bloque " << id << " no encontrado o no está en modo lectura" << endl;
}