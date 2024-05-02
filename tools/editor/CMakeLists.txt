cmake_minimum_required(VERSION 3.16)
project(editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



add_library(editor
    # API
    ${CMAKE_CURRENT_SOURCE_DIR}/../../main/src/vortex.cpp

    # editor requirements
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/ImGui/ImGuiTheme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/UI/UI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/UI/Spinner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/ApplicationGUI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/Image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/src/core/Log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/assets/icons.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/ContentBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/ProjectViewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/TemplateManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/ModuleManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/ProjectSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/editor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Utils/ModuleDetails/ModuleDetails.cpp
)


target_include_directories(editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# editor requirements
if (NOT TARGET launcher)
find_package(Vulkan REQUIRED)
option(SPDLOG_INSTALL "Enable installation of spdlog" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/spdlog spdlog_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/lib/glfw glfw_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/lib/glm glm_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/lib/imgui imgui_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/lib/stb-image stb_image_build)
target_link_libraries(editor PUBLIC Vulkan::Vulkan)
target_link_libraries(editor PUBLIC glfw)
target_link_libraries(editor PUBLIC imgui)
target_link_libraries(editor PUBLIC glm)
target_link_libraries(editor PUBLIC stbimage)
target_link_libraries(editor PUBLIC spdlog)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/icon.png DESTINATION /usr/include/vortex)