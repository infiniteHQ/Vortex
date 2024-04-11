cmake_minimum_required(VERSION 3.0)
project(module)

# Sources du plugin
set(SOURCES
    main.cpp
    src/ToolchainInstance.cpp
    #src/wins/UI_VolatileTasks.cpp
    #src/wins/UI_TasksEditor.cpp                - Use the Tasks module
    #src/wins/UI_SnapshotUtility.cpp
    #src/wins/UI_ParameterWindow.cpp
    #src/wins/UI_ManualBuild.cpp
    src/wins/UI_MainSettings.cpp
    #src/wins/UI_FullBuild.cpp
    #src/wins/UI_CurrentToolchainPreview.cpp    - Use the Reporter module
    src/wins/UI_ContentWindow.cpp
    #src/wins/UI_AssetsViewer.cpp
)

# Création de la bibliothèque partagée (.so)
add_library(module SHARED ${SOURCES})

# Option de compilation
target_compile_options(module PRIVATE -Wall -Wextra)
