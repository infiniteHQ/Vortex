
cmake_minimum_required(VERSION 3.16)
project(vortex)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Liste des fichiers sources pour la bibliothèque partagée
file(GLOB_RECURSE VORTEX_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")
#file(GLOB_RECURSE VORTEXCORE_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/core/*.cpp")
#file(GLOB_RECURSE VORTEXPROJECT_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/projects/*.cpp")
file(GLOB_RECURSE VORTEXUTILS_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/utils/*.cpp")
file(GLOB_RECURSE VORTEXEDITOR_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tools/editor/*.cpp")

# Création de la bibliothèque partagée
add_library(vortex_shared SHARED 
${VORTEX_SHARED_SOURCES}
${VORTEXEDITOR_SHARED_SOURCES}
${VORTEXCORE_SHARED_SOURCES}
${VORTEXPROJECT_SHARED_SOURCES}
${VORTEXUTILS_SHARED_SOURCES}
${IMGUI_SHARED_SOURCES}
)

# Définir les chemins d'inclusion pour la bibliothèque partagée
target_include_directories(vortex_shared PUBLIC
    #${CMAKE_CURRENT_SOURCE_DIR}/main/core/scripts
    #${CMAKE_CURRENT_SOURCE_DIR}/main/core/toolchains
    #${CMAKE_CURRENT_SOURCE_DIR}/main/core/hosts
    #${CMAKE_CURRENT_SOURCE_DIR}/main/core/gpos
    #${CMAKE_CURRENT_SOURCE_DIR}/main/core/disthost
    ${CMAKE_CURRENT_SOURCE_DIR}/main/projects
    ${CMAKE_CURRENT_SOURCE_DIR}/main/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main/src
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/editor
)

# Ajouter d'autres dépendances ou configurations pour la bibliothèque partagée

# Liste des fichiers sources pour l'exécutable principal
file(GLOB_RECURSE VORTEX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")


# Add editor
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/editor editor_build)
target_link_libraries(vortex_shared PUBLIC editor)


# Trouvez la bibliothèque nlohmann/json installée
find_package(nlohmann_json REQUIRED)

# Liens avec la bibliothèque nlohmann/json
target_link_libraries(vortex_shared PRIVATE nlohmann_json::nlohmann_json)


# Création de l'exécutable principal
add_executable(vortex ${VORTEX_SOURCES} main.cpp)

# Liaison avec la bibliothèque partagée
target_link_libraries(vortex PRIVATE vortex_shared)

# Ajouter d'autres dépendances ou configurations pour l'exécutable principal

# Installation de l'exécutable dans /bin
install(TARGETS vortex DESTINATION /bin)
install(TARGETS vortex DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# Installation de la bibliothèque partagée dans /lib
install(TARGETS vortex_shared LIBRARY DESTINATION /lib)
