# A null exemple for API checking.
cmake_minimum_required(VERSION 3.16)
project(vortex)

add_executable(vortex

    # Scripts sources
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/scripts/src/script_utils.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/toolchains/toolchain_prebuild.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/toolchains/toolchain_build.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/toolchains/toolchain_postbuild.cpp

    # Toolchain sources
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/toolchains/src/toolchain_findpackages.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/toolchains/src/toolchain_findtasklists.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/toolchains/src/toolchain_snapshot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/toolchains/src/toolchain_utils.cpp

    # Host sources
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/hosts/host_prebuild.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/hosts/host_build.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/hosts/host_postbuild.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/hosts/host_snapshot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/hosts/host_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/hosts/host_findpackages.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/hosts/host_findtasklists.cpp

    # GPOS sources
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/gpos/gpos_findpackages.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/gpos/gpos_findtasklists.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/gpos/gpos_utils.cpp


    # Dist Toolchain sources
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/core/disthost/disthost_testingchroot.cpp

    # Project managment sources
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/refresh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/update.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/register_disttoolchain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/register_host.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/register_toolchain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/register_script.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/register_package.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/register_gpos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/projects/register_disthost.cpp
    # Utils sources
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/utils/extract_package.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/utils/replace_placeholders.cpp


    # Base of vortex
    ${CMAKE_CURRENT_SOURCE_DIR}/vortex.cpp
	main.cpp
)


# Add editor
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/editor editor_build)
target_link_libraries(vortex PUBLIC editor)


# Trouvez la bibliothèque nlohmann/json installée
find_package(nlohmann_json REQUIRED)

# Liens avec la bibliothèque nlohmann/json
target_link_libraries(vortex PRIVATE nlohmann_json::nlohmann_json)


# Install executable to /bin
install(TARGETS vortex DESTINATION /bin)