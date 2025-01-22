cmake_minimum_required(VERSION 3.16)
project(vortex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(VORTEX_VERSION "1.0")

file(GLOB_RECURSE VORTEX_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")
add_library(vortex_shared SHARED ${VORTEX_SHARED_SOURCES})

target_include_directories(vortex_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main/src
)

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ui/crash_handler ui_crash_handler_build)
target_link_libraries(vortex_shared PUBLIC crash_handler)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ui/editor ui_editor_build)
target_link_libraries(vortex_shared PUBLIC editor)

target_link_libraries(vortex_shared PUBLIC cherry)

file(GLOB_RECURSE VORTEX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")
add_executable(vortex ${VORTEX_SOURCES} main.cpp)

target_link_libraries(vortex PRIVATE vortex_shared)

if(UNIX AND NOT APPLE)
    set(INSTALL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/${VORTEX_VERSION})
    set(INSTALL_MAIN_DIR ${INSTALL_BASE_DIR}/)
    set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_LIB_DIR ${INSTALL_BASE_DIR}/lib)
    set(INSTALL_INCLUDE_DIR ${INSTALL_BASE_DIR}/include)
elseif(WIN32)
    set(INSTALL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/${VORTEX_VERSION})
    set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_LIB_DIR ${INSTALL_BASE_DIR}/lib)
    set(INSTALL_MAIN_DIR ${INSTALL_BASE_DIR}/)
    set(INSTALL_INCLUDE_DIR ${INSTALL_BASE_DIR}/include)
endif()

install(TARGETS vortex DESTINATION ${INSTALL_BIN_DIR})

install(TARGETS vortex_shared LIBRARY DESTINATION ${INSTALL_LIB_DIR})

install(TARGETS spdlog
    LIBRARY DESTINATION ${INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${INSTALL_BIN_DIR})

install(TARGETS cherry
    LIBRARY DESTINATION ${INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
    RUNTIME DESTINATION ${INSTALL_BIN_DIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/main/include/
    DESTINATION ${INSTALL_INCLUDE_DIR}/main/include/
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "*.inl"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ui/editor/assets/
    DESTINATION ${INSTALL_BIN_DIR}/
    FILES_MATCHING PATTERN "*"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ui/crash_handler/assets/
    DESTINATION ${INSTALL_BIN_DIR}/
    FILES_MATCHING PATTERN "*"
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/desktop/vortex_desktop.desktop
    DESTINATION ${INSTALL_MAIN_DIR}/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/vortex_banner.png
    DESTINATION ${INSTALL_MAIN_DIR}/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/build/handle_crash.sh
    DESTINATION ${INSTALL_BIN_DIR}/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/build/handle_crash.bat
    DESTINATION ${INSTALL_BIN_DIR}/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json
    DESTINATION ${INSTALL_MAIN_DIR}/)
