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
)

set(HEADERS include)

add_executable(BD_1 ${SOURCES} ${HEADERS})

target_include_directories(BD_1 PUBLIC include)

