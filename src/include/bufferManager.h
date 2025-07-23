
#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "gestorBloques.h"
#include "query.h"
#include "bloque.h"
#include <string>
#include <list>
#include <unordered_map>
#include <vector>
#include <queue>
using namespace std;

// Enum para las políticas de reemplazo
enum class ReplacementPolicy {
    LRU,
    CLOCK
};

class bufferManager {
    gestorBloques gb;
    Query query;
    string consulta;
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
        // Cola de requests para este frame
        queue<tuple<string, bool, int>> requestQueue; // <mode, dirty, last_accessed>
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
    void writeToDisk(int block_id);

public:
    bufferManager(int frames, ReplacementPolicy policy = ReplacementPolicy::LRU);
    void setQuery(const Query& query);
    Query getQuery() const;
    void setReplacementPolicy(ReplacementPolicy policy);
    void agregarGestorBloques(int id, bloque b);
    void agregarBufferPool(int id, bloque b, const string& mode, bool pinned = false);
    void mostrarEstadoBufferPool() const;
    void unpinBlock(int id);
    bool bloqueEnBuffer(int id) const;
    void pinBlock(int id);
    ReplacementPolicy getCurrentPolicy() const { return current_policy; }
    void mostrarContenidoBloque(int id) const;
    bool handleFullBufferCLOCK(int new_block_id, const string& mode, bool pinned);
    bool handleFullBufferLRU(int new_block_id, const string& mode, bool pinned);

};
#endif // !BUFFERMANAGER_H