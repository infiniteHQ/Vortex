cmake_minimum_required(VERSION 3.0)
project(NomDuModule)

set(SOURCES
    src/main.cpp
)

add_library(NomDuModule SHARED ${SOURCES})

# Spécifiez les dossiers d'inclusion si nécessaire
target_include_directories(NomDuModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Spécifiez les bibliothèques à lier si nécessaire
target_link_libraries(NomDuModule AutreBibliothèque)

# Optionnel : Installation
install(TARGETS NomDuModule
        LIBRARY DESTINATION /chemin/vers/dossier/librairies
)
