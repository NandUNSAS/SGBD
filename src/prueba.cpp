#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <bitset>
#include <algorithm>

using namespace std;

class Bucket {
    int depth, size;
    map<int, string> values;
public:
    Bucket(int depth, int size);
    int insert(int key, string value);
    int remove(int key);
    void search(int key);
    int isFull();
    int isEmpty();
    int getDepth();
    int increaseDepth();
    int decreaseDepth();
    map<int, string> copy();
    void clear();
    void display();
};

class Directory {
    int global_depth, bucket_size;
    vector<Bucket*> buckets;
    int hash(int n);
    int pairIndex(int bucket_no, int depth);
    void grow();
    void shrink();
    void split(int bucket_no);
    void merge(int bucket_no);
    string bucket_id(int n);
public:
    Directory(int depth, int bucket_size);
    void insert(int key, string value, bool reinserted);
    void remove(int key, int mode);
    void search(int key);
    void display(bool duplicates);
};

void menu();

int main() {
    int bucket_size, initial_global_depth;
    int key, mode;
    string choice, value;

    cout << "=== SIMULADOR DE HASHING EXTENDIBLE ===" << endl;
    cout << "Ingrese capacidad del bucket: ";
    cin >> bucket_size;
    cout << "Ingrese profundidad global inicial: ";
    cin >> initial_global_depth;

    Directory d(initial_global_depth, bucket_size);
    cout << endl << "Directorio inicializado" << endl;

    menu();

    do {
        cout << endl << ">>> ";
        cin >> choice;
        if (choice == "insert") {
            cin >> key >> value;
            d.insert(key, value, 0);
        }
        else if (choice == "delete") {
            cin >> key >> mode;
            d.remove(key, mode);
        }
        else if (choice == "search") {
            cin >> key;
            d.search(key);
        }
        else if (choice == "display") {
            d.display(true); // Mostrar todos los buckets, incluyendo duplicados
        }
        else if (choice == "help") {
            menu();
        }
    } while (choice != "exit");

    return 0;
}

void menu() {
    cout << "\n--------------------" << endl;
    cout << "Hashing Extendible:" << endl;
    cout << "insert <key> <value>  - Insertar un nuevo valor" << endl;
    cout << "delete <key> <mode>   - Eliminar un valor (0: lazy, 1: merge empty, 2: merge and shrink)" << endl;
    cout << "update <key> <value>  - Actualizar un valor" << endl;
    cout << "display               - Mostrar estructura completa" << endl;
    cout << "help                  - Mostrar este menu" << endl;
    cout << "exit                  - Salir del programa" << endl;
    cout << "--------------------" << endl;
}

// Implementación de Directory
Directory::Directory(int depth, int bucket_size) {
    this->global_depth = depth;
    this->bucket_size = bucket_size;
    for (int i = 0; i < (1 << depth); i++) {
        buckets.push_back(new Bucket(depth, bucket_size));
    }
}

int Directory::hash(int n) {
    return n & ((1 << global_depth) - 1);
}

int Directory::pairIndex(int bucket_no, int depth) {
    return bucket_no ^ (1 << (depth - 1));
}

void Directory::grow() {
    for (int i = 0; i < (1 << global_depth); i++) {
        buckets.push_back(buckets[i]);
    }
    global_depth++;
}

void Directory::shrink() {
    bool can_shrink = true;
    for (auto bucket : buckets) {
        if (bucket->getDepth() == global_depth) {
            can_shrink = false;
            break;
        }
    }
    if (can_shrink) {
        global_depth--;
        for (int i = 0; i < (1 << global_depth); i++) {
            buckets.pop_back();
        }
    }
}

void Directory::split(int bucket_no) {
    int local_depth = buckets[bucket_no]->increaseDepth();
    if (local_depth > global_depth) {
        grow();
    }

    int pair_index = pairIndex(bucket_no, local_depth);
    buckets[pair_index] = new Bucket(local_depth, bucket_size);

    auto temp = buckets[bucket_no]->copy();
    buckets[bucket_no]->clear();

    int index_diff = 1 << local_depth;
    int dir_size = 1 << global_depth;

    for (int i = pair_index - index_diff; i >= 0; i -= index_diff) {
        buckets[i] = buckets[pair_index];
    }
    for (int i = pair_index + index_diff; i < dir_size; i += index_diff) {
        buckets[i] = buckets[pair_index];
    }

    for (auto& entry : temp) {
        insert(entry.first, entry.second, true);
    }
}

void Directory::merge(int bucket_no) {
    int local_depth = buckets[bucket_no]->getDepth();
    int pair_index = pairIndex(bucket_no, local_depth);

    if (buckets[pair_index]->getDepth() == local_depth) {
        buckets[pair_index]->decreaseDepth();
        delete buckets[bucket_no];
        
        int index_diff = 1 << local_depth;
        int dir_size = 1 << global_depth;
        
        buckets[bucket_no] = buckets[pair_index];
        for (int i = bucket_no - index_diff; i >= 0; i -= index_diff) {
            buckets[i] = buckets[pair_index];
        }
        for (int i = bucket_no + index_diff; i < dir_size; i += index_diff) {
            buckets[i] = buckets[pair_index];
        }
    }
}

string Directory::bucket_id(int n) {
    string s;
    for (int i = global_depth - 1; i >= 0; i--) {
        s += ((n >> i) & 1) ? "1" : "0";
    }
    return s;
}

void Directory::insert(int key, string value, bool reinserted) {
    int bucket_no = hash(key);
    int status = buckets[bucket_no]->insert(key, value);
    
    if (status == 1 && !reinserted) {
        cout  << key <<" -> "  << bucket_id(bucket_no) << endl;
    }
    else if (status == 0) {
        split(bucket_no);
        insert(key, value, reinserted);
    }
    else if (status == -1) {
        cout << "Key " << key << " ya existe en bucket " << bucket_id(bucket_no) << endl;
    }
}

void Directory::remove(int key, int mode) {
    int bucket_no = hash(key);
    if (buckets[bucket_no]->remove(key)) {
        cout << "Eliminado key " << key << " de bucket " << bucket_id(bucket_no) << endl;
    }
    if (mode > 0) {
        if (buckets[bucket_no]->isEmpty() && buckets[bucket_no]->getDepth() > 1) {
            merge(bucket_no);
        }
    }
    if (mode > 1) {
        shrink();
    }
}

void Directory::search(int key) {
    int bucket_no = hash(key);
    cout << "Buscando key " << key << " en bucket " << bucket_id(bucket_no) << endl;
    buckets[bucket_no]->search(key);
}

void Directory::display(bool duplicates) {
    cout << "\nDEPTH GLOBAL: " << global_depth << endl;
    cout << buckets.size() << endl << endl;

    map<string, bool> shown;
    
    for (int i = 0; i < buckets.size(); i++) {
        string id = bucket_id(i);
        
        if (duplicates || shown.find(id) == shown.end()) {
            shown[id] = true;
            
            cout << i << ": [" << id << "] -> Bucket [depth=" << buckets[i]->getDepth() << ", elements={";
            buckets[i]->display();
            cout << "}]" << endl;
        }
    }
}

// Implementación de Bucket
Bucket::Bucket(int depth, int size) : depth(depth), size(size) {}

int Bucket::insert(int key, string value) {
    if (values.find(key) != values.end()) {
        return -1; // Ya existe
    }
    if (isFull()) {
        return 0; // Bucket lleno
    }
    values[key] = value;
    return 1; // Insertado correctamente
}

int Bucket::remove(int key) {
    auto it = values.find(key);
    if (it != values.end()) {
        values.erase(it);
        return 1;
    }
    cout << "No se puede eliminar: key no existe" << endl;
    return 0;
}

void Bucket::search(int key) {
    auto it = values.find(key);
    if (it != values.end()) {
        cout << "Valor encontrado: " << it->second << endl;
    } else {
        cout << "Key no encontrada" << endl;
    }
}

int Bucket::isFull() {
    return values.size() >= size;
}

int Bucket::isEmpty() {
    return values.empty();
}

int Bucket::getDepth() {
    return depth;
}

int Bucket::increaseDepth() {
    return ++depth;
}

int Bucket::decreaseDepth() {
    return --depth;
}

map<int, string> Bucket::copy() {
    return map<int, string>(values);
}

void Bucket::clear() {
    values.clear();
}

void Bucket::display() {
    bool first = true;
    for (auto& entry : values) {
        if (!first) cout << ", ";
        cout << entry.first;
        first = false;
    }
}