# CMakeLists.txt pour le crash_handler
cmake_minimum_required(VERSION 3.16)
project(crash_handler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(crash_handler
    # crash_handler requirements
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/ImGui/ImGuiTheme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/UI/UI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/UI/Spinner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/ApplicationGUI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/Image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/components/windows/windows.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/components/buttons/buttons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/src/core/Log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/assets/icons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/Crash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/src/crash_handler.cpp
)

target_include_directories(crash_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# crash_handler requirements
if (NOT TARGET editor AND NOT TARGET launcher)
find_package(Vulkan REQUIRED)
option(SPDLOG_INSTALL "Enable installation of spdlog" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/spdlog spdlog_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/lib/glfw glfw_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/lib/glm glm_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/lib/imgui imgui_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/lib/stb-image stb_image_build)
target_link_libraries(crash_handler PUBLIC Vulkan::Vulkan)
target_link_libraries(crash_handler PUBLIC glfw)
target_link_libraries(crash_handler PUBLIC imgui)
target_link_libraries(crash_handler PUBLIC glm)
target_link_libraries(crash_handler PUBLIC stbimage)
target_link_libraries(crash_handler PUBLIC spdlog)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/uikit/platform/GUI/editor/icon.png DESTINATION /usr/include/vortex)
