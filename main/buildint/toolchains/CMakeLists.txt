cmake_minimum_required(VERSION 3.0)
project(module)


file(GLOB_RECURSE VORTEX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../../src/*.cpp")


# Sources du plugin
set(SOURCES
    main.cpp
    src/instances/toolchainInstance/ToolchainInstance.cpp
    #src/wins/UI_VolatileTasks.cpp
    #src/wins/UI_TasksEditor.cpp                - Use the Tasks module
    #src/wins/UI_SnapshotUtility.cpp
    #src/wins/UI_ParameterWindow.cpp
    #src/wins/UI_ManualBuild.cpp
    src/instances/toolchainInstance/wins/UI_MainSettings.cpp
    #src/wins/UI_FullBuild.cpp
    #src/wins/UI_CurrentToolchainPreview.cpp    - Use the Reporter module
    src/instances/toolchainInstance/wins/UI_ContentWindow.cpp
    #src/wins/UI_AssetsViewer.cpp

    ${VORTEX_SOURCES}
)

# Création de la bibliothèque partagée (.so)
add_library(module SHARED ${SOURCES})

# Option de compilation
target_compile_options(module PRIVATE -Wall -Wextra)
