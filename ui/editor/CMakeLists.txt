cmake_minimum_required(VERSION 3.16)
project(editor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
target_compile_definitions(editor PUBLIC CHERRY_IMPORT_DLL)
target_link_libraries(editor PUBLIC cherry)

target_include_directories(editor PUBLIC "${PROJECT_SOURCE_DIR}/lib/spdlog/include/spdlog")
if(UNIX AND NOT APPLE)
    target_link_libraries(editor PRIVATE crypto)
endif()


file(GLOB_RECURSE ASSET_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"
)    

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/remove_resources_if_exists.cmake
"if(EXISTS \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources\")\n"
"    file(REMOVE_RECURSE \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources\")\n"
"endif()\n"
)

add_custom_target(sync_assets_editor ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Synchronizing assets..."
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/remove_resources_if_exists.cmake
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    COMMAND ${CMAKE_COMMAND} -E echo "Assets synchronized."
    DEPENDS ${ASSET_FILES}
)


add_dependencies(editor sync_assets_editor)

