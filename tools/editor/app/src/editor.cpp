#include "../include/editor.h"

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

static void handleLogUtility()
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
void EditorLayer::menubar(const std::shared_ptr<EditorLayer> &exampleLayer, UIKit::Application *app)
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Exit"))
            handleExit(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Project Settings", "Main configurations of this project", &exampleLayer->ShowProjectSettings))
            handleProjectSettings();
        ImGui::Separator();
        if (ImGui::MenuItem("Logs Utility", "Overview of all logs", &exampleLayer->ShowLogUtility))
            handleLogUtility();
        ImGui::Separator();
        if (ImGui::MenuItem("Manage plugins", "Add, remove, edit plugins of this project"))
            handleManagePlugins();
        if (ImGui::MenuItem("Manage modules", "Manage modules loaded/registered", &exampleLayer->ShowModulesManager))
            handleManageModules(exampleLayer->ShowModulesManager);
        if (ImGui::MenuItem("Templates modules", "Create, add template in your project", &exampleLayer->ShowTemplateManager))
            handleManageModules(exampleLayer->ShowTemplateManager);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Window"))
    {
        if (ImGui::MenuItem("Show bottom toolbar", "Get some useful tools in a bottom bar."))
            handleShowBottomToolbar();
        if (ImGui::MenuItem("Show simplified header", "Reduce the size of header"))
            handleShowSimplifiedHeader(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools"))
    {
        if (ImGui::MenuItem("Content Browser", "Project file manager", &exampleLayer->ShowContentBrowser))
            handleContentBrowser(exampleLayer->ShowContentBrowser);
        if (ImGui::MenuItem("Project Viewer", "Project component manager", &exampleLayer->ShowProjectViewer))
            handleProjectViewer(exampleLayer->ShowProjectViewer);
        ImGui::EndMenu();
    }

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

/*

 - Creation de projet via launcher (avec la possibilité de charger les templates "project")
 - Creation de templates de modules, etc.. "modules:package", "module:toolchain"


*/

UIKit::Application *CreateEditor(int argc, char **argv)
{
  int port = atoi(argv[1]);

  std::shared_ptr<EditorLayer> applayer = std::make_shared<EditorLayer>();
  applayer->m_ctx = VortexMaker::GetCurrentContext();

  UIKit::ApplicationSpecification spec;
  std::string name = applayer->m_ctx->name + " - Vortex Editor";
  spec.Name = name;
  spec.CustomTitlebar = true;
  spec.IconPath = "icon.png";

  UIKit::Application *app = new UIKit::Application(spec);

  app->PushLayer(applayer);
  app->SetMenubarCallback([app, applayer]()
                          { applayer->menubar(applayer, app); });

  return app;
}

int VortexMaker::VortexEditor(int argc, char **argv)
  {
    return UIKit::ThirdMain(argc, argv, CreateEditor);
  }