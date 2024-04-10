cmake_minimum_required(VERSION 3.0)
project(pluginone)

# Sources du plugin
set(SOURCES
    main.cpp
)

# Création de la bibliothèque partagée (.so)
add_library(pluginone SHARED ${SOURCES})

# Option de compilation
target_compile_options(pluginone PRIVATE -Wall -Wextra)
