#include "../include/bufferManager.h"
#include "../include/bloque.h"
#include "../include/funciones.h"
#include <iostream>
#include <fstream>
#include <limits>

using namespace std;
const string RUTASB = "../../rutas_sectores/cilindroMedio.txt";
const string SALIDA = "../../salidaTerminal.txt";
// Constructor

bufferManager::bufferManager()
    : time_counter(0), capacity(0), clock_hand(0), current_policy(ReplacementPolicy::LRU) {
    // No inicializa frames porque capacity es 0
}

bufferManager::bufferManager(int frames, ReplacementPolicy policy) 
    : capacity(frames), time_counter(0), clock_hand(0), current_policy(policy) {
    for (int i = 0; i < capacity; i++) {
        this->frames.emplace_back(i);
    }
    cout << "BufferManager creado con " << frames << " frames y política ";
    cout << (current_policy == ReplacementPolicy::LRU ? "LRU" : "CLOCK") << endl;
}

void bufferManager::setQuery(const Query& q) {
    query = q;
}

Query bufferManager::getQuery() const {
    return query;
}

//funciones recientes
bool bufferManager::handleFullBufferCLOCK(int new_block_id, const string& mode, bool pinned) {
    while (true) {
        Frame& current_frame = frames[clock_hand];
        
        cout << "\n CLOCK apuntando al frame " << current_frame.frame_id << endl;

        // Procesar solicitudes pendientes si hay
        if (!current_frame.requestQueue.empty()) {
            auto request = current_frame.requestQueue.front();

            if (current_frame.pin_count > 0) {
                char respuesta;
                cout << "Request pendiente (modo: " << get<0>(request)
                     << "). pin_count = " << current_frame.pin_count << endl;

                respuesta = 's';
                if (tolower(respuesta) == 's') {
                    if(get<0>(request) == "read") {
                        cout << "Contenido del bloque " << current_frame.block_id << ":" << endl;
                        gb.mostrarBloque(current_frame.block_id);  // Mostrar el contenido del bloque actual
                        
                        ///
                        if(query.getTieneWhere()){
                            string atributo = query.getAtributo();
                            string valor = query.getValor();
                            cout << "getwhere true" << endl;
                        }

                        else {
                            int id = current_frame.block_id;
                            bloque b = gb.obtenerBloque(id);
                            string contenido = b.getContenido();
                            ofstream archivo(SALIDA, ios::app);
                            if (!archivo.is_open()) {
                                cerr << "Error: el archivo no está abierto." << endl;
                            }
                            archivo << contenido << endl;
                            archivo.close();
                        }
                        
                        ///
                    } else {
                        bloque& blk = gb.obtenerBloque(current_frame.block_id);
                        blk.escribirPaginaSimulada("Datos modificados en frame " + to_string(current_frame.frame_id));  // Escribir datos al bloque
                        writeToDisk(current_frame.block_id);  // Escribir a disco si está dirty
                        current_frame.dirty = false; // Limpiar dirty bit
                        cout << "Bloque marcado como dirty false" << endl;
                    }
                    current_frame.pin_count--;
                    current_frame.requestQueue.pop();

                    cout << " pin_count decremented a " << current_frame.pin_count << endl;

                    // Si pin_count llega a 0, limpiamos el bit de referencia
                    if (current_frame.pin_count == 0) {
                        current_frame.reference_bit = false;
                        cout << " reference_bit puesto en false" << endl;
                    }

                    // Si quedan más requests en la cola, actualizamos el frame
                    if (!current_frame.requestQueue.empty()) {
                        auto next_request = current_frame.requestQueue.front();
                        current_frame.mode = get<0>(next_request);
                        current_frame.dirty = get<1>(next_request);
                        cout << " Frame actualizado con siguiente request en cola." << endl;
                    }
                    
                    // En cualquier caso, avanzamos al siguiente frame
                    clock_hand = (clock_hand + 1) % frames.size();
                    mostrarEstadoBufferPool();
                    continue;
                } else {
                    cout << " Usuario rechazó procesar el request. Avanzando." << endl;
                    clock_hand = (clock_hand + 1) % frames.size();
                    mostrarEstadoBufferPool();
                    continue;
                }
            }
        }

        // Verificamos si es candidato a reemplazo
        if (current_frame.pin_count == 0 && current_frame.reference_bit == false && !current_frame.pinned) {
            cout << " Frame " << current_frame.frame_id << " elegido para reemplazo." << endl;

            // Cargar nuevo bloque en el frame
            string rutaBloque;
            try {
                rutaBloque = obtenerRutaPorId(RUTASB, new_block_id);
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
                return false;
            }
            bloque b;
            b.inicializarBloque(new_block_id, rutaBloque);
            gb.agregarBloque(new_block_id, b);  // Agregar bloque al gestor de bloques

            current_frame.block_id = new_block_id;
            current_frame.mode = mode;
            current_frame.pin_count = 1;
            current_frame.pinned = pinned;
            current_frame.dirty = (mode == "write");
            current_frame.reference_bit = true;

            // Agregar el nuevo request
            current_frame.requestQueue = queue<tuple<string, bool, int>>(); // Limpiar cola
            current_frame.requestQueue.push(make_tuple(mode, (mode == "write"), time_counter));
            
            cout << " Nuevo bloque " << new_block_id << " cargado en frame " << current_frame.frame_id << endl;
            // 
            // Avanzamos el puntero CLOCK
            clock_hand = (clock_hand + 1) % frames.size();
            return true;
        } else {
            // Si no se puede reemplazar, seguimos al siguiente frame
            if (current_frame.reference_bit) {
                cout << " Segunda oportunidad: limpiando reference_bit del frame " << current_frame.frame_id << endl;
                current_frame.reference_bit = false;
            } else {
                cout << " No elegible aún para reemplazo. Avanzando." << endl;
            }

            clock_hand = (clock_hand + 1) % frames.size();
        }
    }
}


bool bufferManager::handleFullBufferLRU(int new_block_id, const string& mode, bool pinned,bloque& bloqueAgregar) {
    while (true) { 
        int lru_index = -1;
        int min_time = numeric_limits<int>::max();
        
        // Encontrar el frame LRU
        for (int i = 0; i < frames.size(); i++) {
            if (frames[i].last_accessed < min_time && frames[i].last_accessed != -1) {
                min_time = frames[i].last_accessed;
                lru_index = i;
            }
        }
        
        if (lru_index == -1) {
            cerr << "ERROR: No se pudo encontrar frame LRU" << endl;
            return false;
        }
        
        Frame& lru_frame = frames[lru_index];
        if (!lru_frame.requestQueue.empty()) {
            auto request = lru_frame.requestQueue.front();
            cout << "\n--- Procesando Request en Frame " << lru_frame.frame_id << " ---" << endl;
            cout << "Modo: " << get<0>(request) <<endl;
            cout << "Requests en cola: " << lru_frame.requestQueue.size() << endl;

            if (lru_frame.pin_count > 0) {
                // 4. Preguntar al usuario
                char respuesta;
                /*
                cout << "Request pendiente (modo: " << get<0>(request)
                     <<"\n¿Procesar request? (s/n): ";
                cin >> respuesta;
                */
                respuesta = 's';
                if (tolower(respuesta) == 's') {
                    cout << "tabla: " << query.getTabla() << endl;
                    string tipo_consulta;

                    if (get<0>(request) == "read") {
                        cout << "Contenido del bloque " << lru_frame.block_id << ":" << endl;
                        gb.mostrarBloque(lru_frame.block_id);  // Mostrar el contenido del bloque actual
                        

                    } else {
                        bloque& blk = gb.obtenerBloque(lru_frame.block_id);
                        blk.escribirPaginaSimulada("Datos modificados en frame " + to_string(lru_frame.frame_id));  // Escribir datos al bloque
                        writeToDisk(lru_frame.block_id);  // Escribir a disco si está dirty
                        lru_frame.dirty = false; // Limpiar dirty bit
                        cout << "Bloque marcado como dirty false" << endl;
                    }
                    lru_frame.pin_count--;
                    lru_frame.requestQueue.pop();  // Procesar el request
                    cout << "Request procesado. PinCount decrementado a: " << lru_frame.pin_count << endl;
                    // si quedan más requests en la cola, actualizamos el frame
                    if (!lru_frame.requestQueue.empty()) {
                        auto next_request = lru_frame.requestQueue.front();
                        lru_frame.mode = get<0>(next_request);
                        lru_frame.dirty = get<1>(next_request);
                        lru_frame.last_accessed = get<2>(next_request);
                        cout << "Frame actualizado con request de la cola: " << get<0>(request) << endl;
                    }else {
                        lru_frame.mode = "none"; // No hay más requests
                        lru_frame.last_accessed = -1; // Reset last_accessed
                        cout << "No quedan más requests en la cola." << endl;
                    }
                    mostrarEstadoBufferPool();
                    // 5. Verificar si el frame es candidato a reemplazo
                    if (lru_frame.pin_count == 0 && !lru_frame.pinned) {
                        cout << "Frame " << lru_frame.frame_id << " elegido para reemplazo." << endl;
                        
                        // Cargar nuevo bloque en el frame
                        string rutaBloque;
                        try {
                            rutaBloque = obtenerRutaPorId(RUTASB, new_block_id);
                        } catch (const exception& e) {
                            cerr << "Error: " << e.what() << endl;
                            return false;
                        }
                        
                        //bloque b;
                        //b.inicializarBloque(new_block_id, rutaBloque);
                        gb.agregarBloque(new_block_id, bloqueAgregar);
                        
                        lru_frame.block_id = new_block_id;
                        lru_frame.pin_count = 1; // Asignar pin_count inicial
                        lru_frame.mode = mode;
                        lru_frame.pinned = pinned;
                        lru_frame.dirty = (mode == "write");
                        lru_frame.last_accessed = ++time_counter;
                        
                        lru_frame.requestQueue = queue<tuple<string, bool, int>>(); // Limpiar cola de requests 
                        lru_frame.requestQueue.push(make_tuple(mode, (mode == "write"), time_counter));  // Agregar el request a la cola
                        
                        cout << "Nuevo bloque " << new_block_id << " cargado en frame " << lru_index << endl;
                        
                        return true;  // Reemplazo exitoso
                    }
                    continue;
                }
                else {
                    cout << "Reemplazo cancelado por el usuario" << endl;
                    continue;
                }
            }
            
        }
        
    }
}


void bufferManager::setReplacementPolicy(ReplacementPolicy policy) {
    current_policy = policy;
    cout << "Política de reemplazo cambiada a ";
    cout << (current_policy == ReplacementPolicy::LRU ? "LRU" : "CLOCK") << endl;
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
    int aux_time_counter = -1;
    
    // 1. Buscar si el bloque ya está en buffer
    for (auto& frame : frames) {
        if (frame.block_id == id) {
            frame.pin_count++; // Incrementar pin_count
            frame.pinned = pinned; // Actualizar estado de pinned
            frame.reference_bit = true; // Para CLOCK
            if(frame.last_accessed == -1) {
                frame.last_accessed = time_counter;
                frame.requestQueue.push(make_tuple(mode, (mode=="write"), time_counter));
                cout << "Request agregado a frame existente " << frame.frame_id << endl;
                return;
            }
            frame.requestQueue.push(make_tuple(mode, (mode=="write"), time_counter));
            if(clock_hand == frame.frame_id) {
                // Si el frame actual es el que apunta CLOCK, avanzamos el puntero
                clock_hand = (clock_hand + 1) % frames.size();
            }
            cout << "Request agregado a frame existente " << frame.frame_id << endl;
            return;
        }
    }
    
    // 2. Buscar frame libre
    for (auto& frame : frames) {
        if (frame.block_id == -1) {
            frame.requestQueue.push(make_tuple(mode, (mode=="write"), time_counter));
            
            frame.block_id = id;
            frame.pin_count = 1; // Inicializar pin_count
            frame.dirty = (mode == "write");
            frame.pinned = pinned;
            frame.mode = mode;
            frame.last_accessed = time_counter; // Actualizar last_accessed
            frame.reference_bit = true; // Para CLOCK
            gb.agregarBloque(id, b);
            cout << "Bloque cargado en frame libre " << frame.frame_id << endl;
            return;
        }
    }
    
    // 3. Buffer lleno - manejar según LRU
    cout << "\n--- BUFFER LLENO ---" << endl;
    cout << "Todos los frames tienen pinCount > 0" << endl;
    time_counter--;
        // Llamar a la función correcta según la política
    if (current_policy == ReplacementPolicy::LRU) {
        while (!handleFullBufferLRU(id, mode, pinned,b)) {
            cout << "Buscando siguiente frame candidato (LRU)..." << endl;
        }
    } else if (current_policy == ReplacementPolicy::CLOCK) {
        while (!handleFullBufferCLOCK(id, mode, pinned)) {
            cout << "Buscando siguiente frame candidato (CLOCK)..." << endl;
        }
    } else {
        cerr << "ERROR: Política de reemplazo desconocida" << endl;
    }
}

// Mostrar estado del buffer pool
void bufferManager::mostrarEstadoBufferPool() const {
    cout << "\n--- Estado del Buffer Pool (" 
         << (current_policy == ReplacementPolicy::LRU ? "LRU" : "CLOCK") << ") ---" << endl;
         
    if (current_policy == ReplacementPolicy::CLOCK) {
        cout << "Clock hand posición: " << clock_hand << endl;
    }
    
    cout << "Frame | Block | PinCount | Dirty | Pinned | Request | ";
    if (current_policy == ReplacementPolicy::LRU) {
        cout << "LastUsed | ";
    } else {
        cout << "RefBit | ";
    }
    cout<<endl;
    //cout << "Mode" << endl;
    cout << "-----------------------------------------------------------" << endl;
    
    for (const auto& frame : frames) {
        cout << "  " << frame.frame_id << "   |  " 
             << (frame.block_id == -1 ? "-" : to_string(frame.block_id)) << "   |    "
             << frame.pin_count << "     |  "
             << (frame.dirty ? "Si" : "No") << "    |  "
             << (frame.pinned ? "Si" : "No") << "    |    ";

        // Construir string con la cola de requests en formato L/E
        // Para no modificar la cola original, hacemos una copia temporal
        queue<tuple<string, bool, int>> tempQueue = frame.requestQueue;
        string requestsStr;
        while (!tempQueue.empty()) {
            const auto& req = tempQueue.front();
            const string& mode = std::get<0>(req);
            if (mode == "read") {
                requestsStr += "L";
            } else if (mode == "write") {
                requestsStr += "E";
            } else {
                requestsStr += "?"; // caso raro o inesperado
            }
            tempQueue.pop();
        }

        cout << requestsStr << "     |  ";        
        
        if (current_policy == ReplacementPolicy::LRU) {
            cout << frame.last_accessed << "    |  ";
        } else {
            cout << (frame.reference_bit ? "1" : "0") << "     |  ";
        }
        cout<<endl;
        //cout << frame.mode << endl;
    }
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