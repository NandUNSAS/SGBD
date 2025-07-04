#include "../include/bufferManager.h"
#include "../include/bloque.h"
#include <iostream>
#include <limits>

using namespace std;

// Constructor
bufferManager::bufferManager(int frames, ReplacementPolicy policy) 
    : capacity(frames), time_counter(0), clock_hand(0), current_policy(policy) {
    for (int i = 0; i < capacity; i++) {
        this->frames.emplace_back(i); //crea un vector de frames de tam "capacity".
    }
    cout << "BufferManager creado con " << frames << " frames y política ";
    cout << (current_policy == ReplacementPolicy::LRU ? "LRU" : "CLOCK") << endl;
}

void bufferManager::setReplacementPolicy(ReplacementPolicy policy) {
    current_policy = policy;
    cout << "Política de reemplazo cambiada a ";
    cout << (current_policy == ReplacementPolicy::LRU ? "LRU" : "CLOCK") << endl;
}

void bufferManager::actualizarFramesDesdeHistorial() {
    for (auto& frame : frames) {
        int id = frame.frame_id;

        // Verificamos si hay historial para este frame
        auto it = historial_frames_por_frame.find(id);
        if (it != historial_frames_por_frame.end() && !it->second.empty()) {
            const Frame& primer_historial = it->second[0];

            // Solo actualizamos los valores excepto pin_count
            frame.block_id = primer_historial.block_id;
            frame.dirty = primer_historial.dirty;
            frame.pinned = primer_historial.pinned;
            frame.mode = primer_historial.mode;
            frame.last_accessed = primer_historial.last_accessed;
            frame.reference_bit = primer_historial.reference_bit;
            // ⚠️ NO actualizamos pin_count
        }
    }
}

int bufferManager::findLRUFrame() {
    while (true) {
        int lru_index = -1;
        int min_last_used = numeric_limits<int>::max();

        // Paso 1: Buscar frame no pineado con menor last_accessed
        for (const auto& frame : frames) {
            if (!frame.pinned && frame.block_id != -1 && frame.last_accessed < min_last_used) {
                min_last_used = frame.last_accessed;
                lru_index = frame.frame_id;
            }
        }

        if (lru_index == -1) {
            cerr << "No hay ningún frame candidato para reemplazo (todos pineados)" << endl;
            return -1;
        }

        Frame& victim = frames[lru_index];

        // Paso 2: Si está pineado, no podemos hacer nada
        if (victim.pinned) {
            continue; // omitimos y volvemos a buscar
        }

        // Paso 3: Si está dirty, preguntar si se desea escribir
        if (victim.dirty) {
            char respuesta;
            cout << "El bloque " << victim.block_id << " está dirty. "
                 << "¿Deseas escribirlo en disco? (s/n): ";
            cin >> respuesta;

            if (respuesta == 's' || respuesta == 'S') {
                writeToDisk(victim.block_id);
                victim.dirty = false;
            } else {
                cout << "No se escribió en disco. No se puede reemplazar aún." << endl;
                // No podemos continuar con este frame, pasamos al siguiente
                continue;
            }
        }

        // Paso 4: Si dirty ya está limpio, reducir pin_count
        if (victim.pin_count > 0) {
            victim.pin_count--;
            mostrarEstadoBufferPool();
        }

        // Paso 5: Eliminar primer historial de este frame
        auto it = historial_frames_por_frame.find(lru_index);
        if (it != historial_frames_por_frame.end() && !it->second.empty()) {
            it->second.erase(it->second.begin());

            // Si queda historial, actualizamos con el nuevo primero
            if (!it->second.empty()) {
                const Frame& nuevo = it->second.front();

                // Actualizar todos los campos excepto pin_count
                victim.block_id = nuevo.block_id;
                victim.dirty = nuevo.dirty;
                victim.pinned = nuevo.pinned;
                victim.mode = nuevo.mode;
                victim.last_accessed = nuevo.last_accessed;
                victim.reference_bit = nuevo.reference_bit;
            } else {
                // Ya no hay historial: dejar el frame vacío
                cout << "ya no hay historial" << endl;
                victim.block_id = -1;
                victim.dirty = false;
                victim.pin_count = 0;
                victim.pinned = false;
                victim.mode = "none";
                victim.last_accessed = 0;
                victim.reference_bit = false;
            }
            mostrarEstadoBufferPool();
        }
        cout << victim.pin_count << " | " << victim.block_id << endl;
        // Paso 6: Si pin_count ya es 0, devolver el índice
        if (victim.pin_count == 0) {
            return lru_index;
        }
        cout << "entro en bucle" << endl;
        // Si aún no se puede reemplazar, seguir iterando
    }

    // Nunca debería llegar aquí
    return -1;
}


int bufferManager::findClockFrame() {
    int inicio = clock_hand;
    bool dio_vuelta = false;

    while (true) {
        Frame& frame = frames[clock_hand];

        if (!frame.pinned) {
            // Si tiene dirty, se escribe antes de cualquier acción
            if (frame.dirty) {
                char respuesta;
                cout << "El bloque " << frame.block_id << " está dirty. ¿Deseas escribirlo a disco? (s/n): ";
                cin >> respuesta;

                if (respuesta == 's' || respuesta == 'S') {
                    writeToDisk(frame.block_id);
                    frame.dirty = false;
                } else {
                    cout << "No se escribió en disco. No se puede continuar con este frame." << endl;
                    clock_hand = (clock_hand + 1) % capacity;
                    if (clock_hand == inicio) {
                        if (dio_vuelta) break;
                        dio_vuelta = true;
                    }
                    continue;
                }
            }

            // Si dirty ya es 0, se puede descontar pin_count y actualizar historial
            if (frame.pin_count > 0) {
                frame.pin_count--;

                // Actualizar el frame con el siguiente en la cola si existe
                auto& historial = historial_frames_por_frame[clock_hand];
                if (!historial.empty()) {
                    historial.erase(historial.begin());
                    if (!historial.empty()) {
                        const Frame& nuevo = historial.front();
                        frame.block_id = nuevo.block_id;
                        frame.dirty = nuevo.dirty;
                        frame.pinned = nuevo.pinned;
                        frame.mode = nuevo.mode;
                        frame.last_accessed = nuevo.last_accessed;
                        frame.reference_bit = nuevo.reference_bit;
                    } else {
                        // Sin historial
                        frame.block_id = -1;
                        frame.dirty = false;
                        frame.pinned = false;
                        frame.mode = "none";
                        frame.last_accessed = 0;
                        frame.reference_bit = false;
                    }
                }
            }

            // Si pin_count llegó a 0, evaluar refbit
            if (frame.pin_count == 0) {
                if (frame.reference_bit == 0) {
                    // Frame candidato para reemplazo
                    int seleccionado = clock_hand;

                    // Eliminar de la cola y actualizar (nuevamente por seguridad)
                    auto& historial = historial_frames_por_frame[clock_hand];
                    if (!historial.empty()) {
                        historial.erase(historial.begin());
                        if (!historial.empty()) {
                            const Frame& nuevo = historial.front();
                            frame.block_id = nuevo.block_id;
                            frame.dirty = nuevo.dirty;
                            frame.pinned = nuevo.pinned;
                            frame.mode = nuevo.mode;
                            frame.last_accessed = nuevo.last_accessed;
                            frame.reference_bit = nuevo.reference_bit;
                        } else {
                            frame.block_id = -1;
                            frame.dirty = false;
                            frame.pinned = false;
                            frame.mode = "none";
                            frame.last_accessed = 0;
                            frame.reference_bit = false;
                        }
                    }

                    clock_hand = (clock_hand + 1) % capacity;
                    return seleccionado;
                } else {
                    // Dar segunda oportunidad
                    frame.reference_bit = 0;
                }
            }
        }

        // Avanzar la manecilla circularmente
        clock_hand = (clock_hand + 1) % capacity;

        if (clock_hand == inicio) {
            if (dio_vuelta) break;
            dio_vuelta = true;
        }
    }

    cerr << "ERROR: No se encontró ningún frame reemplazable (todos en uso o pineados)." << endl;
    return -1;
}



// Escribir bloque a disco si está dirty
void bufferManager::writeToDisk(int block_id) {
    cout << "Escribiendo bloque " << block_id 
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
            frame.dirty = (mode == "write");
            frame.pinned = pinned;
            frame.last_accessed = time_counter;
            frame.reference_bit = true;
            frame.mode = mode;

            
            historial_frames_por_frame[frame.frame_id].push_back(frame);
            actualizarFramesDesdeHistorial();
            
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

            historial_frames_por_frame[frame.frame_id].push_back(frame);;
            actualizarFramesDesdeHistorial();

            gb.agregarBloque(id, b);
            
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
    
    historial_frames_por_frame[victim.frame_id].push_back(victim);
    actualizarFramesDesdeHistorial();
    gb.agregarBloque(id, b);
    
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
    cout << "Mode | Historial del frame" << endl;

    cout << "----------------------------------------------------------------------------------------------------------" << endl;

    for (const auto& frame : frames) {
        // Imprimir estado actual del frame
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

        cout << frame.mode << " | ";

        // Buscar historial del frame actual
        auto it = historial_frames_por_frame.find(frame.frame_id);
        if (it != historial_frames_por_frame.end() && !it->second.empty()) {
            const auto& historial = it->second;

            // Mostrar el primer registro como "Bloque X (modo)"
            const Frame& primero = historial[0];
            cout << "RE1 " << primero.block_id << " (modo: " << primero.mode << ")";

            // Mostrar los siguientes como RE2(...), RE3(...)
            for (size_t i = 1; i < historial.size(); ++i) {
                const Frame& re = historial[i];
                cout << ", RE" << i + 1 << "("
                     << (re.dirty ? "Si" : "No") << ", "
                     << re.last_accessed << ", "
                     << re.mode << ")";
            }
        } else {
            cout << "Sin historial";
        }

        cout << endl;
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
            } else if (mode == "write") {
                cout << "\n--- Estado después de acceso ---" << endl;
                mostrarEstadoBufferPool();
                
                // Obtener el bloque actual y simular escritura
                bloque& blk = gb.obtenerBloque(id); // Necesitarás implementar este método
                blk.escribirPaginaSimulada("Datos actualizados para bloque " + to_string(id));
                frame.dirty = true;
                
                // Decrementar sin mostrar tabla
                frame.pin_count--;
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