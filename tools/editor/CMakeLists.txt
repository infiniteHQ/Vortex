cmake_minimum_required(VERSION 3.16)
project(editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



add_library(editor
    # API
    ${CMAKE_CURRENT_SOURCE_DIR}/../../vortex.cpp

    # editor requirements
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Platform/GUI/editor/ImGui/ImGuiTheme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Platform/GUI/editor/UI/UI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Platform/GUI/editor/ApplicationGUI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Platform/GUI/editor/Image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Source/editor/Core/Log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Assets/icons.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/ContentBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/core/ProjectViewer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/wins/UI_ContentWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/wins/UI_MainSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/wins/UI_ParameterWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/wins/UI_FullBuild.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/wins/UI_AssetsViewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/wins/UI_SnapshotUtility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/wins/UI_CurrentHostPreview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/wins/UI_TasksEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Host/HostInstance.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/GPOS/GPOSInstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/GPOS/wins/UI_MainSettings.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Assets/Package/PackageInstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Assets/Package/wins/UI_MainSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Assets/Package/wins/UI_CompilationArguments.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Assets/Tasklist/wins/UI_MainSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Assets/Tasklist/TasklistInstance.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/ToolchainInstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_ContentWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_AssetsViewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_MainSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_TasksEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_ManualBuild.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_VolatileTasks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_SnapshotUtility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_CurrentToolchainPreview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Components/Toolchain/wins/UI_ParameterWindow.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/app/instances/Utils/Report/ReportInstance.cpp
)




# editor requirements
find_package(Vulkan REQUIRED)
find_package(spdlog REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw glfw_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm glm_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui imgui_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb-image stb_image_build)
target_link_libraries(editor PUBLIC Vulkan::Vulkan)
target_link_libraries(editor PUBLIC glfw)
target_link_libraries(editor PUBLIC imgui)
target_link_libraries(editor PUBLIC glm)
target_link_libraries(editor PUBLIC stbimage)
target_link_libraries(editor PUBLIC spdlog::spdlog)
