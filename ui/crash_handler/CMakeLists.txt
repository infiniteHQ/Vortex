cmake_minimum_required(VERSION 3.16)
project(crash_handler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT TARGET cherry)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cherry cherry_build)
endif()

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

target_link_libraries(crash_handler PUBLIC cherry)
target_include_directories(crash_handler PUBLIC "${PROJECT_SOURCE_DIR}/lib/spdlog/include/spdlog")

# Step 1: Gather all files from assets directory
file(GLOB_RECURSE ASSET_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"
)

# Step 2: Add a custom target to synchronize assets
add_custom_target(sync_assets ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Synchronizing assets..."
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
    COMMAND ${CMAKE_COMMAND} -E echo "Assets synchronized."
    DEPENDS ${ASSET_FILES}
)

# Step 3: Ensure the launcher target depends on asset synchronization
add_dependencies(crash_handler sync_assets)

