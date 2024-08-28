cmake_minimum_required(VERSION 3.16)
project(vortex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC")
set(VORTEX_VERSION "1.1")

file(GLOB_RECURSE VORTEX_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")

add_library(vortex_shared SHARED ${VORTEX_SHARED_SOURCES})

target_include_directories(vortex_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main/src
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/editor editor_build)
target_link_libraries(vortex_shared PUBLIC editor)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/crash_handler crash_handler_build)
target_link_libraries(vortex_shared PUBLIC crash_handler)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/launcher launcher_build)
target_link_libraries(vortex_shared PUBLIC launcher)

file(GLOB_RECURSE VORTEX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")

add_executable(vortex ${VORTEX_SOURCES} main.cpp)
target_link_libraries(vortex PRIVATE vortex_shared)

add_executable(vx ${VORTEX_SOURCES} bootstrapp.cpp)
target_link_libraries(vx PRIVATE vortex_shared)

set(INSTALL_INCLUDE_DIR /usr/local/include/Vortex/${VORTEX_VERSION}/)

set(INSTALL_BIN_DIR /usr/local/bin/Vortex/${VORTEX_VERSION}/)

install(TARGETS vortex DESTINATION  ${INSTALL_BIN_DIR})
install(TARGETS vx DESTINATION /bin)

install(TARGETS vortex_shared LIBRARY DESTINATION /lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/main/include/
        DESTINATION ${INSTALL_INCLUDE_DIR}/main/include/
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tools/editor/app/
        DESTINATION ${INSTALL_INCLUDE_DIR}/tools/editor/app/
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/
        DESTINATION ${INSTALL_INCLUDE_DIR}/lib
        FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/
        DESTINATION  ${INSTALL_INCLUDE_DIR}/imgs/) 

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/desktop/vortex_desktop.desktop
        DESTINATION /usr/share/applications/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/build/handle_crash.sh
        DESTINATION ${INSTALL_BIN_DIR}/)
