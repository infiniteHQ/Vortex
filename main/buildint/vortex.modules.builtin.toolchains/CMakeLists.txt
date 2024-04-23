cmake_minimum_required(VERSION 3.0)
project(module)

# Inclure la bibliothèque partagée vortex_shared
find_library(VORTEX_SHARED_LIBRARY vortex_shared HINTS /lib)

# Vérifier si la bibliothèque a été trouvée
if(NOT VORTEX_SHARED_LIBRARY)
    message(FATAL_ERROR "Please build/install the vortex core before compiling modules !")
endif()


# Sources du plugin
set(SOURCES
    main.cpp
    src/toolchain.cpp
    src/instances/toolchainInstance/ToolchainRenderInstance.cpp
    src/instances/toolchainInstance/wins/UI_AssetsViewer.cpp
    src/instances/toolchainInstance/wins/UI_ParameterWindow.cpp
    src/instances/toolchainInstance/wins/UI_CurrentToolchainPreview.cpp
    src/instances/toolchainInstance/wins/UI_ContentWindow.cpp
    src/instances/toolchainInstance/wins/UI_VolatileTasks.cpp
    src/instances/toolchainInstance/wins/UI_TasksEditor.cpp
    src/instances/toolchainInstance/wins/UI_FullBuild.cpp
    src/instances/toolchainInstance/wins/UI_ManualBuild.cpp
    src/instances/toolchainInstance/wins/UI_SnapshotUtility.cpp
    src/instances/toolchainInstance/wins/UI_MainSettings.cpp
)

# Création de la bibliothèque partagée (.so)
add_library(module SHARED ${SOURCES})

# Option de compilation
target_compile_options(module PRIVATE -Wall -Wextra)

# Liaison avec la bibliothèque partagée vortex_shared
target_link_libraries(module PRIVATE ${VORTEX_SHARED_LIBRARY})