#include "../include/crash_handler.h"

static void handleExit(UIKit::Application *app)
{
    app->Close();
}

static void handleProjectSettings()
{
}

static void handleManagePlugins()
{
}

static void handleManageModules(bool &showModulesManager)
{
}

static void handleShowBottomToolbar()
{
}

static void handleShowSimplifiedHeader(UIKit::Application *app)
{
    app->m_Specification.CustomTitlebar = !app->m_Specification.CustomTitlebar;
}

static void handleContentBrowser(bool &showContentBrowser)
{
}

static void handleProjectViewer(bool &showProjectViewer)
{
}

static void handleNews()
{
}

static void handleCommunity()
{
}

static void handleTutorials()
{
}

static void handleDocumentation()
{
}

// Main menu function
void CrashHandlerLayer::menubar(const std::shared_ptr<CrashHandlerLayer> &exampleLayer, UIKit::Application *app)
{
    if (ImGui::BeginMenu("Help"))
    {
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
