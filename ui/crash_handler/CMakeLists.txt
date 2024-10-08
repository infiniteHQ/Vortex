cmake_minimum_required(VERSION 3.16)
project(crash_handler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT TARGET cherry)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cherry cherry_build)
endif()

file(GLOB_RECURSE UI_CRASH_HANDLER CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
)

message(STATUS "UI_CRASH_HANDLER sources: ${UI_CRASH_HANDLER}")

if (UI_CRASH_HANDLER)
    add_library(crash_handler STATIC ${UI_CRASH_HANDLER})
else()
    message(FATAL_ERROR "No source files found for the crash_handler target.")
endif()

target_link_libraries(crash_handler PRIVATE cherry)

add_custom_command(TARGET crash_handler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
)
