cmake_minimum_required(VERSION 3.16)
project(vortex)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Liste des fichiers sources pour la bibliothèque partagée
file(GLOB_RECURSE VORTEX_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")
file(GLOB_RECURSE VORTEXEDITOR_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tools/editor/*.cpp")
file(GLOB_RECURSE VORTEXEDITOR_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tools/launcher/*.cpp")

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
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main/src
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/editor
)

# Liste des fichiers sources pour l'exécutable principal
file(GLOB_RECURSE VORTEX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")

# Add editor
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/editor editor_build)
target_link_libraries(vortex_shared PUBLIC editor)

# Add editor
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/launcher launcher_build)
target_link_libraries(vortex_shared PUBLIC launcher)

# Création de l'exécutable principal
add_executable(vortex ${VORTEX_SOURCES} main.cpp)

add_executable(vortex_desktop ${VORTEX_SOURCES} desktop.cpp)

# Liaison avec la bibliothèque partagée
target_link_libraries(vortex PRIVATE vortex_shared)

# Ajouter d'autres dépendances ou configurations pour l'exécutable principal

# Installation de l'exécutable dans /bin
install(TARGETS vortex DESTINATION /bin)
install(TARGETS vortex_desktop DESTINATION /bin)
install(TARGETS vortex DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# Installation de la bibliothèque partagée dans /lib
install(TARGETS vortex_shared LIBRARY DESTINATION /lib)

# Définir le répertoire d'installation pour les fichiers d'en-tête
set(INSTALL_INCLUDE_DIR /usr/include/Vortex)

# Installation des fichiers d'en-tête de /main/include
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/main/include/
        DESTINATION ${INSTALL_INCLUDE_DIR}/main/include/
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

# Installation des fichiers d'en-tête de /tools/editor/app/include
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tools/editor/app/
        DESTINATION ${INSTALL_INCLUDE_DIR}/tools/editor/app/
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

# Installation des fichiers d'en-tête de /lib
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/
        DESTINATION ${INSTALL_INCLUDE_DIR}/lib
        FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/vortex_desktop.desktop
        DESTINATION /usr/share/applications/)