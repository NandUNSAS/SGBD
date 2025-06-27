#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "gestorBloques.h"
#include "bloque.h"
#include <string>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

// Enum para las políticas de reemplazo
enum class ReplacementPolicy {
    LRU,
    CLOCK
};

class bufferManager {
    gestorBloques gb;
    
    // Estructura común para ambos algoritmos
    struct Frame {
        int frame_id;
        int block_id;       // ID del bloque cargado
        int pin_count;
        bool dirty;
        bool pinned;
        string mode;        // "read" o "write"
        
        // Para LRU
        int last_accessed;
        
        // Para Clock
        bool reference_bit;
        
        Frame(int id) : frame_id(id), block_id(-1), pin_count(0), 
                       dirty(false), pinned(false), mode("none"),
                       last_accessed(0), reference_bit(false) {}
    };

    vector<Frame> frames;
    int time_counter;
    int capacity;
    int clock_hand;  // Puntero para algoritmo Clock
    ReplacementPolicy current_policy;

    // Métodos auxiliares
    int findLRUFrame();
    int findClockFrame();
    void writeToDisk(int block_id);

public:
    bufferManager(int frames, ReplacementPolicy policy = ReplacementPolicy::LRU);
    void setReplacementPolicy(ReplacementPolicy policy);
    void agregarGestorBloques(int id, bloque b);
    void agregarBufferPool(int id, bloque b, const string& mode, bool pinned = false);
    void mostrarEstadoBufferPool() const;
    void accederBloque(int id, const string& mode);
    void unpinBlock(int id);
    bool bloqueEnBuffer(int id) const;
    void pinBlock(int id);
    ReplacementPolicy getCurrentPolicy() const { return current_policy; }
};
#endif // !BUFFERMANAGER_H