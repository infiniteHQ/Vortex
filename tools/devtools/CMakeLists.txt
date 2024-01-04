cmake_minimum_required(VERSION 3.16)
project(devtools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



add_library(devtools
    # API
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vortex.cpp

    # Devtools requirements
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Platform/GUI/Devtools/ImGui/ImGuiTheme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Platform/GUI/Devtools/UI/UI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Platform/GUI/Devtools/ApplicationGUI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Platform/GUI/Devtools/Image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Source/Devtools/Core/Log.cpp
)




# Devtools requirements
find_package(Vulkan REQUIRED)
find_package(spdlog REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw glfw_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm glm_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui imgui_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb-image stb_image_build)
target_link_libraries(devtools PUBLIC Vulkan::Vulkan)
target_link_libraries(devtools PUBLIC glfw)
target_link_libraries(devtools PUBLIC imgui)
target_link_libraries(devtools PUBLIC glm)
target_link_libraries(devtools PUBLIC stbimage)
target_link_libraries(devtools PUBLIC spdlog::spdlog)
