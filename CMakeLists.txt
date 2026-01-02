cmake_minimum_required(VERSION 3.16)
project(vortex)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/utf-8)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC")
endif()

# ---------------------------------------------------------------------------
# Version / Build infos
# ---------------------------------------------------------------------------
set(VORTEX_VERSION "1.0")
set(VORTEX_FULL_VERSION "1.0.build1")
set(VORTEX_BUILDID "1")
set(VORTEX_BUILDNAME "1.0 [Build 1]")
set(VORTEX_DIST "beta")
set(VORTEX_BUILD "dev")

if(VORTEX_BUILD STREQUAL "prod")
    add_compile_definitions(BUILD_STR="01")
    string(TIMESTAMP BUILD_DATE "%b %d, %Y - %H:%M" UTC)
    add_compile_definitions(BUILD_DATE_STR="${BUILD_DATE}")
    add_compile_definitions(BUILD_DIST="${BUILD_DATE}")
    add_compile_definitions(VORTEX_BUILD="prod")
    add_compile_definitions(VORTEX_BUILDID="${VORTEX_BUILDID}")
    add_compile_definitions(VORTEX_BUILDNAME="${VORTEX_BUILDNAME}")

    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    add_compile_definitions(GIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
else()
    add_compile_definitions(BUILD_DATE_STR="DEV_BUILD")
    add_compile_definitions(BUILD_DIST="DEV_BUILD")
    add_compile_definitions(GIT_COMMIT_HASH="0000000")
    add_compile_definitions(VORTEX_BUILD="dev")
    add_compile_definitions(VORTEX_BUILDID="${VORTEX_BUILDID}")
    add_compile_definitions(VORTEX_BUILDNAME="${VORTEX_BUILDNAME}")
endif()

# ---------------------------------------------------------------------------
# Deps
# ---------------------------------------------------------------------------
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include")

# ---------------------------------------------------------------------------
# Cherry UI Library
# ---------------------------------------------------------------------------
option(CHERRY_STATIC "Link Cherry statically"                            OFF)
option(CHERRY_DEBUG "Enable Cherry debug tools"                         OFF)

option(CHERRY_IMGUI_EXPORT "Disable workspace saves"                    ON)
option(CHERRY_PACKAGE "Enable packaging features with pyinstaller"      OFF)
option(CHERRY_MULTIAPP "Use multiple apps in one executable"            ON)

option(CHERRY_ENABLE_NET "Enable net features"                          OFF)
option(CHERRY_ENABLE_AUDIO "Enable audio features"                      OFF)
option(CHERRY_ENABLE_KIT "Enable base component kit"                    OFF)
option(CHERRY_ENABLE_LINUXDRM "Enable Linux Direct Rendering Manager"   OFF)
option(CHERRY_ENABLE_NODES "Disable nodal systems"                      OFF)
option(CHERRY_ENABLE_WORKSPACESAVES "Disable workspace saves"           OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/cherry cherry_build)

file(GLOB_RECURSE VORTEX_ALL_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/editor/app/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/crash_handler/src/*.cpp"
)

# ---------------------------------------------------------------------------
# Core lib
# ---------------------------------------------------------------------------
add_library(vortex_shared SHARED ${VORTEX_ALL_SOURCES})

target_include_directories(vortex_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/editor/app/src
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/crash_handler/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include
)

target_link_libraries(vortex_shared PUBLIC cherry)

if(UNIX AND NOT APPLE)
    target_link_libraries(vortex_shared PRIVATE crypto)
endif()

# ---------------------------------------------------------------------------
# Executables
# ---------------------------------------------------------------------------
add_executable(vortex main.cpp)
target_link_libraries(vortex PUBLIC vortex_shared)

add_executable(vortex_utils utils.cpp)
target_link_libraries(vortex_utils PUBLIC vortex_shared)


if(UNIX AND NOT APPLE)
    find_package(CURL REQUIRED)
    target_link_libraries(vortex PRIVATE CURL::libcurl)
endif()

target_compile_definitions(vortex_shared
    PUBLIC CHERRY_MULTIAPP
)

target_compile_definitions(vortex
    PRIVATE CHERRY_MULTIAPP
)

target_compile_definitions(vortex_utils
    PRIVATE CHERRY_MULTIAPP
)

# ---------------------------------------------------------------------------
# Assets
# ---------------------------------------------------------------------------
add_custom_target(sync_vortex_assets ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Synchronizing All UI Assets..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/ui/editor/assets" "${CMAKE_BINARY_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/ui/crash_handler/assets" "${CMAKE_BINARY_DIR}/bin"
)
add_dependencies(vortex_shared sync_vortex_assets)

# ---------------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------------
set(INSTALL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/${VORTEX_FULL_VERSION})
set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
set(INSTALL_LIB_DIR ${INSTALL_BASE_DIR}/bin)

set_target_properties(vortex PROPERTIES
    INSTALL_RPATH "${INSTALL_LIB_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(TARGETS vortex DESTINATION ${INSTALL_BIN_DIR})
install(TARGETS vortex_shared
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
    LIBRARY DESTINATION ${INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR} 
)
install(TARGETS cherry
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
    LIBRARY DESTINATION ${INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR} 
)