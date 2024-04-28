#include "editor.h"

// Helper functions for menu items

void handleExit(UIKit::Application* app) {
    app->Close();
}

void handleProjectSettings() {
    // Behavior
}

void handleManagePlugins() {
    // Behavior
}

void handleManageModules(bool& showModulesManager) {
    // Behavior
}

void handleShowBottomToolbar() {
    // Behavior
}

void handleShowSimplifiedHeader(UIKit::Application* app) {
    app->m_Specification.CustomTitlebar = !app->m_Specification.CustomTitlebar;
}

void handleContentBrowser(bool& showContentBrowser) {
    // Behavior
}

void handleProjectViewer(bool& showProjectViewer) {
    // Behavior
}

void handleNews() {
    // Behavior
}

void handleCommunity() {
    // Behavior
}

void handleTutorials() {
    // Behavior
}

void handleDocumentation() {
    // Behavior
}

// Main menu function
void ApplicationLayer::menubar(const std::shared_ptr<ApplicationLayer>& exampleLayer, UIKit::Application* app) {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit"))
            handleExit(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Project Settings", "Main configurations of this project"))
            handleProjectSettings();
        ImGui::Separator();
        if (ImGui::MenuItem("Manage plugins", "Add, remove, edit plugins of this project"))
            handleManagePlugins();
        if (ImGui::MenuItem("Manage modules", "Project file manager", &exampleLayer->ShowModulesManager))
            handleManageModules(exampleLayer->ShowModulesManager);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Window")) {
        if (ImGui::MenuItem("Show bottom toolbar", "Get some useful tools in a bottom bar."))
            handleShowBottomToolbar();
        if (ImGui::MenuItem("Show simplified header", "Reduce the size of header"))
            handleShowSimplifiedHeader(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools")) {
        if (ImGui::MenuItem("Content Browser", "Project file manager", &exampleLayer->ShowContentBrowser))
            handleContentBrowser(exampleLayer->ShowContentBrowser);
        if (ImGui::MenuItem("Project Viewer", "Project component manager", &exampleLayer->ShowProjectViewer))
            handleProjectViewer(exampleLayer->ShowProjectViewer);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("News", "Get latest Vortex news"))
            handleNews();
        if (ImGui::MenuItem("Community", "Join a community of creators"))
            handleCommunity();
        if (ImGui::MenuItem("Tutorials", "Get bunch of tutorials"))
            handleTutorials();
        if (ImGui::MenuItem("Documentation", "See official documentation of Vortex Maker"))
            handleDocumentation();
        ImGui::Separator();
        if (ImGui::MenuItem("Exit"))
            handleExit(app);
        ImGui::EndMenu();
    }
}
