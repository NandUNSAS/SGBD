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
        
        /*
        Frame(int id) : frame_id(id), block_id(-1), pin_count(0), 
                       dirty(false), pinned(false), mode("none"),
                       last_accessed(0), reference_bit(false) {}
        */
        Frame(int id) : 
            frame_id(id),     // Asigna el ID recibido como parámetro
            block_id(-1),     // -1 indica que el frame está vacío (no contiene bloque)
            pin_count(0),     // Contador de pines en 0 (nadie lo está usando)
            dirty(false),     // No modificado desde la última escritura a disco
            pinned(false),    // No está bloqueado para reemplazo
            mode("none"),     // Sin modo de acceso definido
            last_accessed(0), // Para LRU: nunca accedido inicialmente
            reference_bit(false) {} // Para CLOCK: bit de referencia apagado
    };

    vector<Frame> frames;
    int time_counter; //temporizador para LRU
    int capacity;
    int clock_hand;  // Puntero para algoritmo Clock
    ReplacementPolicy current_policy; //indica la polotica de reemplazo

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
    void mostrarContenidoBloque(int id) const;
};
#endif // !BUFFERMANAGER_H