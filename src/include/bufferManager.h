#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "gestorBloques.h"
#include "bloque.h"
#include <string>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

class bufferManager {
    gestorBloques gb;
    
    // Estructura para el LRU
    struct Frame {
        int frame_id;
        int block_id;       // ID del bloque cargado
        int pin_count;
        bool dirty;
        bool pinned;
        int last_accessed;  // Contador para LRU
        string mode;        // "read" o "write"
        
        Frame(int id) : frame_id(id), block_id(-1), pin_count(0), 
                       dirty(false), pinned(false), last_accessed(0), mode("none") {}
    };

    vector<Frame> frames;
    int time_counter;
    int capacity;

    // Métodos auxiliares
    int findLRUFrame();
    void writeToDisk(int block_id);

public:
    bufferManager(int frames);
    void agregarGestorBloques(int id, bloque b);
    void agregarBufferPool(int id, bloque b, const string& mode);
    void mostrarEstadoBufferPool() const;
    void accederBloque(int id, const string& mode);
    void unpinBlock(int id);
    bool bloqueEnBuffer(int id) const;
    void pinBlock(int id);
    int findFrameWithLowestPinCount();
};

#endif // !BUFFERMANAGER_H