cmake_minimum_required(VERSION 3.16)
project(editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT TARGET cherry)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cherry cherry_build)
endif()

file(GLOB_RECURSE UI_EDITOR CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
)

message(STATUS "UI_EDITOR sources: ${UI_EDITOR}")

if (UI_EDITOR)
    add_library(editor STATIC ${UI_EDITOR})
else()
    message(FATAL_ERROR "No source files found for the editor target.")
endif()

target_link_libraries(editor PUBLIC cherry)
target_include_directories(editor PUBLIC "${PROJECT_SOURCE_DIR}/lib/spdlog/include/spdlog")

add_custom_command(TARGET editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
)
