cmake_minimum_required(VERSION 3.16)
project(launcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



add_library(launcher
    # API
    ${CMAKE_CURRENT_SOURCE_DIR}/../../main/src/vortex.cpp

    # launcher requirements
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/platform/GUI/editor/ImGui/ImGuiTheme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/platform/GUI/editor/UI/UI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/platform/GUI/editor/UI/Spinner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/platform/GUI/editor/ApplicationGUI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/platform/GUI/editor/Image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/components/windows/windows.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/components/buttons/buttons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/src/core/Log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/assets/icons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/ProjectManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/SystemSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../editor/app/core/ContentBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/Details.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/LauncherLogUtility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/launcher.cpp
)


target_include_directories(launcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# launcher requirements
if (NOT TARGET editor AND NOT TARGET crash_handler)
find_package(Vulkan REQUIRED)
# Find SDL2
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Link SDL2 library
target_link_libraries(launcher PUBLIC ${SDL2_LIBRARIES})
option(SPDLOG_INSTALL "Enable installation of spdlog" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/spdlog spdlog_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/lib/glfw glfw_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/lib/glm glm_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/lib/imgui imgui_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/lib/stb-image stb_image_build)
target_link_libraries(launcher PUBLIC Vulkan::Vulkan)
target_link_libraries(launcher PUBLIC imgui)
target_link_libraries(launcher PUBLIC glm)
target_link_libraries(launcher PUBLIC stbimage)
target_link_libraries(launcher PUBLIC spdlog)
endif()


install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit_sdl/platform/GUI/editor/icon.png DESTINATION /usr/include/vortex)