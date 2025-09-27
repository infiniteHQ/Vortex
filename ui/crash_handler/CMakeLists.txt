cmake_minimum_required(VERSION 3.16)
project(crash_handler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE UI_CRASH_HANDLER CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

message(STATUS "UI_CRASH_HANDLER sources: ${UI_CRASH_HANDLER}")

if (UI_CRASH_HANDLER)
    add_library(crash_handler STATIC ${UI_CRASH_HANDLER})
else()
    message(FATAL_ERROR "No source files found for the crash_handler target.")
endif()
target_compile_definitions(crash_handler PUBLIC CHERRY_IMPORT_DLL)
target_link_libraries(crash_handler PUBLIC cherry)

target_include_directories(crash_handler PUBLIC "${PROJECT_SOURCE_DIR}/lib/spdlog/include/spdlog")
if(UNIX AND NOT APPLE)
    target_link_libraries(crash_handler PRIVATE crypto)
endif()

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/remove_resources_if_exists.cmake
"if(EXISTS \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ch_resources\")\n"
"    file(REMOVE_RECURSE \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ch_resources\")\n"
"endif()\n"
)

add_custom_target(sync_assets ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Synchronizing assets..."
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/remove_resources_if_exists.cmake
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    COMMAND ${CMAKE_COMMAND} -E echo "Assets synchronized."
    DEPENDS ${ASSET_FILES}
)

add_dependencies(crash_handler sync_assets)

