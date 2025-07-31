set(SOURCES
        main.cpp
        implementacion/funciones.cpp
        implementacion/disco.cpp
        implementacion/gestorAlmacenamiento.cpp
        implementacion/controladorDisco.cpp
        implementacion/query.cpp
        implementacion/funcionesBloque.cpp
        implementacion/funciones_menu.cpp
        implementacion/insertar.cpp
        implementacion/bloque.cpp
        implementacion/bufferManager.cpp
        implementacion/gestorBloque.cpp
        implementacion/query.cpp
        implementacion/esquema.cpp
        implementacion/controlador_Query.cpp
        implementacion/hashing_extendible.cpp
)

set(HEADERS include)

add_executable(SGBD ${SOURCES} ${HEADERS})

target_include_directories(SGBD PUBLIC include)

