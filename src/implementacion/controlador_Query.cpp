#include "../include/controlador_Query.h"
#include "../include/insertar.h"
#include <sstream>
#include <iomanip>
#include <fstream>

const string RUTASB = "../../rutas_sectores/cilindroMedio.txt";

ControladorQuery::ControladorQuery(){

}

void ControladorQuery::setTipoConsulta(string& tipo){
    tipoConsulta = tipo;
}

int obtenerIdBloqueVacio(const string& rutaArchivo) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << rutaArchivo << endl;
        return -1;  // Valor de error
    }

    string linea;
    int id = 1;  // Empezamos desde 1

    while (getline(archivo, linea)) {
        if (linea.empty()) {
            ++id;
            continue;
        }

        size_t pos = linea.find('#');
        if (pos != string::npos) {
            string estadoStr = linea.substr(0, pos);
            if (estadoStr == "0") {
                archivo.close();
                return id;
            }
        }

        ++id;
    }

    archivo.close();
    return -1;  // No se encontró ningún bloque libre
}

void ControladorQuery::efectuarConsulta(Query& _query, string rutaBloques){
    if(tipoConsulta == "INSERT"){
        int id = obtenerIdBloqueVacio(rutaBloques);
        realizarConsultaInsert(_query, id);
    }
    else if(tipoConsulta == "SELECT"){
        //ControladorQuery controlador;
        //controlador.generarIndicePostulantes();
        // Aquí se implementaría la lógica para consultas SELECT
        cout << "Consulta SELECT no implementada aún." << endl;
    }
    else if(tipoConsulta == "UPDATE"){
        // Aquí se implementaría la lógica para consultas UPDATE
        cout << "Consulta UPDATE no implementada aún." << endl;
    }
    else if(tipoConsulta == "DELETE"){
        // Aquí se implementaría la lógica para consultas DELETE
        cout << "Consulta DELETE no implementada aún." << endl;
    }
    else{
        cout << "Tipo de consulta no reconocido." << endl;
    }
}

void modifcarCabecera(string& cabecera, int tamañoRegistro, int cantidadMaximaRegistros){

}

void obtenerDesplazamientoInserción(){

}
string obtenerCabecera(string& contenidoBloque){
    return contenidoBloque;
}

int obtenerDesplazamiento(string& cabecera){
    int desplazamiento = 0;
    return desplazamiento;
}

void desplazamientoInserción(int desplazamiento){

}
void ControladorQuery::agregarInserción(bloque& _bloque, string& registro){
    string contenidoBloque = _bloque.getContenido();
    string cabecera = obtenerCabecera(contenidoBloque);
    esquema = query.getEsquema();
    int tamañoRegistro = esquema.calcularTamanoRegistro();
    int cantidadMaximaRegistros = tamañoRegistro/tamañoRegistro;
    modifcarCabecera(cabecera, tamañoRegistro, cantidadMaximaRegistros);
    int desplazamiento = obtenerDesplazamiento(cabecera);
    desplazamientoInserción(desplazamiento);

    // Extraer ID del postulante
    size_t hash_pos = registro.find('#');
    if(hash_pos == string::npos) return;
    
    int id = stoi(registro.substr(0, hash_pos));
    
    // Actualizar índice
    actualizarIndiceInsercion(id, _bloque.getIdBloque(), desplazamiento);
}

string ControladorQuery::crearRegistro() {
    const vector<CampoEsquema>& campos = query.getEsquema().getCampos();
    const vector<string>& valores = query.getValores();
    string registroInsercion;

    if (valores.size() != campos.size()) {
        cerr << "Error: cantidad de valores no coincide con el esquema." << endl;
        return "";
    }

    for (size_t i = 0; i < campos.size(); ++i) {
        string valor = valores[i];
        string tipo = campos[i].tipo;
        int tamano = campos[i].tamano;

        if (tipo == "string") {
            if ((int)valor.size() > tamano) {
                valor = valor.substr(0, tamano);  // Truncar
            } else {
                valor.append(tamano - valor.size(), ' ');  // Padding con espacios
            }
        } else if (tipo == "int") {
            valor = to_string(stoi(valor));
            if ((int)valor.size() > tamano) {
                valor = valor.substr(0, tamano);
            } else {
                valor.insert(valor.begin(), tamano - valor.size(), '0');
            }
        } else if (tipo == "float") {
            stringstream ss;
            ss << fixed << setprecision(2) << stof(valor);
            valor = ss.str();
            if ((int)valor.size() > tamano) {
                valor = valor.substr(0, tamano);
            } else {
                valor.insert(valor.begin(), tamano - valor.size(), '0');
            }
        }

        registroInsercion += valor;

        // Agregar '#' solo si no es el último campo
        if (i != campos.size() - 1) {
            registroInsercion += '#';
        }
    }

    return registroInsercion;
}

void ControladorQuery::realizarConsultaInsert(Query _query, int id){
    //string nombreTabla = query.getTabla();
    esquema = query.getEsquema();
    _bloque = gb.getBloque(id);
    string registro = crearRegistro();
    agregarInserción(_bloque, registro);
}

void ControladorQuery::generarIndicePostulantes() {
    postulantes_index = ExtendibleHash(2, 4, "indice_postulantes.dat");
    
    int block_id = 1;
    while(true) {
        string ruta = obtenerRutaPorId(RUTASB, block_id);
        if(ruta == "no encontrado") break;
        
        bloque b;
        b.inicializarBloque(block_id, ruta);
        string contenido = b.getContenido();
        
        // Saltar cabecera
        size_t pos = contenido.find('\n');
        if(pos == string::npos) {
            block_id++;
            continue;
        }
        
        // Procesar registros
        int offset = 0;
        size_t inicio = pos + 1;
        while(inicio < contenido.size()) {
            pos = contenido.find('\n', inicio);
            if(pos == string::npos) pos = contenido.size();
            
            string registro = contenido.substr(inicio, pos - inicio);
            if(!registro.empty()) {
                // Extraer ID (primer campo antes del #)
                size_t hash_pos = registro.find('#');
                if(hash_pos != string::npos) {
                    try {
                        int id = stoi(registro.substr(0, hash_pos));
                        postulantes_index.insert(id, {block_id, offset});
                    } catch(...) {
                        // Ignorar registros mal formados
                    }
                }
                
                // Actualizar offset para el próximo registro
                offset = pos - inicio;
            }
            
            inicio = pos + 1;
        }
        
        block_id++;
    }
    
    postulantes_index.saveToFile();
}

std::string ControladorQuery::buscarPostulantePorID(int id) {
    if(!postulantes_index.loadFromFile()) {
        generarIndicePostulantes(); // Regenerar índice si no existe
    }
    
    BlockPosition pos = postulantes_index.search(id);
    if(!pos.isValid()) {
        return "Postulante no encontrado";
    }
    
    // Obtener bloque
    string ruta = obtenerRutaPorId(RUTASB, pos.block_id);
    if(ruta == "no encontrado") {
        return "Bloque no encontrado";
    }
    
    bloque b;
    b.inicializarBloque(pos.block_id, ruta);
    string contenido = b.getContenido();
    
    // Saltar cabecera
    size_t inicio = contenido.find('\n') + 1;
    if(inicio == string::npos) {
        return "Formato de bloque inválido";
    }
    
    // Buscar registro en la posición indicada
    size_t registro_inicio = inicio + pos.offset;
    if(registro_inicio >= contenido.size()) {
        return "Desplazamiento inválido";
    }
    
    size_t registro_fin = contenido.find('\n', registro_inicio);
    if(registro_fin == string::npos) {
        registro_fin = contenido.size();
    }
    
    return contenido.substr(registro_inicio, registro_fin - registro_inicio);
}

void ControladorQuery::actualizarIndiceInsercion(int id, int block_id, int offset) {
    postulantes_index.insert(id, {block_id, offset});
    postulantes_index.saveToFile();
}