cmake_minimum_required(VERSION 3.16)
project(vortex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(VORTEX_VERSION "1.0")
set(VORTEX_FULL_VERSION "1.0.1")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/${VORTEX_VERSION} CACHE PATH "Installation directory" FORCE)

set(VORTEX_DIST "beta")
set(VORTEX_BUILD "dev")

if(VORTEX_BUILD STREQUAL "prod")
    string(TIMESTAMP BUILD_DATE "%b %d, %Y - %H:%M" UTC)
    add_compile_definitions(BUILD_DATE_STR="${BUILD_DATE}")
    add_compile_definitions(BUILD_DIST="${BUILD_DATE}")

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
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include")

file(GLOB_RECURSE VORTEX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")

if(NOT VORTEX_DISABLE_UI)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ui/crash_handler ui_crash_handler_build)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ui/editor ui_editor_build)
    list(APPEND VORTEX_SOURCES
        $<TARGET_OBJECTS:crash_handler>
        $<TARGET_OBJECTS:editor>
    )
else()
    message("UI Disabled")
endif()

add_executable(vortex ${VORTEX_SOURCES} main.cpp)

add_executable(vortex_utils utils.cpp)

target_include_directories(vortex PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main/src
)

if(NOT VORTEX_DISABLE_UI)
    target_link_libraries(vortex PRIVATE crash_handler editor)
endif()

if(UNIX AND NOT APPLE)
    find_package(CURL REQUIRED)
    target_link_libraries(vortex PRIVATE CURL::libcurl)
endif()

if(UNIX AND NOT APPLE)
    set(INSTALL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/${VORTEX_VERSION})
    set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_MAIN_DIR ${INSTALL_BASE_DIR}/)
elseif(WIN32)
    set(INSTALL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/${VORTEX_VERSION})
    set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_MAIN_DIR ${INSTALL_BASE_DIR}/)
endif()

set_target_properties(vortex PROPERTIES
    INSTALL_RPATH "${INSTALL_BASE_DIR}/lib"
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(TARGETS vortex DESTINATION ${INSTALL_BIN_DIR})
install(TARGETS vortex_utils DESTINATION ${INSTALL_BIN_DIR})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/images/vortex_banner.png DESTINATION ${INSTALL_MAIN_DIR}/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/build/handle_crash.sh DESTINATION ${INSTALL_BIN_DIR}/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/build/handle_crash.bat DESTINATION ${INSTALL_BIN_DIR}/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json DESTINATION ${INSTALL_MAIN_DIR}/)
