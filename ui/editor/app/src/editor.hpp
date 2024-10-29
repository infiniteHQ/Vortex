#include "../../../../lib/cherry/cherry.hpp"

// Static windows
#include "../core/welcome/welcome.hpp"
#include "../core/modules_utility/modules_utility.hpp"
#include "../core/templates_utility/templates_utility.hpp"
#include "../core/project_settings/project_settings.hpp"

// Instanciable windows
#include "../instances/instance.h"
#include "../instances/content_browser/content_browser.hpp"
#include "../instances/logs_utility/logs_utility.hpp"

using namespace VortexMaker;

#ifndef EDITOR_H
#define EDITOR_H

static std::vector<std::shared_ptr<VortexEditor::ContentBrowserAppWindow>> c_ContentBrowserInstances;
static std::vector<std::shared_ptr<VortexEditor::LogsUtilityAppWindow>> c_LogsUtilityInstances;

class EditorLayer : public Cherry::Layer
{
public:
  EditorLayer() {};

  void menubar(const std::shared_ptr<EditorLayer> &applayer, Cherry::Application *app);
  void framebar(const std::shared_ptr<EditorLayer> &applayer, Cherry::Application *app);

  void OnFinish() override
  {
  }

  void OnUIRender() override
  {
    PushStyle();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
    ImGui::RenderNotifications();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
    
    PopStyle();
  }
  void PushStyle()
  {
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 11.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 11.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(9.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 7.0f);
  }

  void PopStyle()
  {
    ImGui::PopStyleVar(8);
  }
  VxContext *m_ctx;

  // Global TaskProcessor instance
  // TaskProcessor taskProcessor;

  std::thread receiveThread;
  bool ShowContentBrowser = true;
  bool ShowProjectViewer = false;
  bool ShowModulesManager = false;
  bool ShowTemplateManager = false;
  bool ShowProjectSettings = false;
  bool ShowLogUtility = false;

  // Instance factory;

private:
  std::vector<std::string> instanciedWindowsNames;
};

class Editor
{
public:
  Editor()
  {
    m_WelcomeAppWindow = VortexEditor::WelcomeWindow::Create("?loc:loc.window_names.welcome");
    Cherry::AddAppWindow(m_WelcomeAppWindow->GetAppWindow());

    m_ModuleUtilityAppWindow = VortexEditor::ModulesUtilityAppWindow::Create("Modules utily");
    m_ModuleUtilityAppWindow->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(m_ModuleUtilityAppWindow->GetAppWindow());

    m_ProjectSettingsAppWindow = VortexEditor::ProjectSettingsAppWindow::Create("Project settings");
    m_ProjectSettingsAppWindow->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(m_ProjectSettingsAppWindow->GetAppWindow());

    m_TemplatesUtilityAppWindow = VortexEditor::TemplatesUtilityAppWindow::Create("Templates utility");
    m_TemplatesUtilityAppWindow->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(m_TemplatesUtilityAppWindow->GetAppWindow());
  };

  void SetTemplatesUtilityVisibility(const bool &visibility)
  {
    m_TemplatesUtilityAppWindow->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetTemplatesUtilityVisibility()
  {
    return m_TemplatesUtilityAppWindow->GetAppWindow()->m_Visible;
  }

  void SetProjectSettingsVisibility(const bool &visibility)
  {
    m_ProjectSettingsAppWindow->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetProjectSettingsVisibility()
  {
    return m_ProjectSettingsAppWindow->GetAppWindow()->m_Visible;
  }


  void SetModuleUtilityVisibility(const bool &visibility)
  {
    m_ModuleUtilityAppWindow->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetModuleUtilityVisibility()
  {
    return m_ModuleUtilityAppWindow->GetAppWindow()->m_Visible;
  }

  void SetWelcomeWindowVisibility(const bool &visibility)
  {
    m_WelcomeAppWindow->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetWelcomeWindowVisibility()
  {
    return m_WelcomeAppWindow->GetAppWindow()->m_Visible;
  }

  void SpawnContentBrowser()
  {
    std::string label = "logs-" + std::to_string(c_LogsUtilityInstances.size() + 1);
    std::shared_ptr<VortexEditor::ContentBrowserAppWindow> ContentBrowser = std::make_shared<VortexEditor::ContentBrowserAppWindow>(label.c_str(), "/home/diego");
    Cherry::AddAppWindow(ContentBrowser->GetAppWindow());
    c_ContentBrowserInstances.push_back(ContentBrowser);
  }

  void SpawnLogsUtility()
  {
    std::string label = "logs-" + std::to_string(c_LogsUtilityInstances.size() + 1);
    std::shared_ptr<VortexEditor::LogsUtilityAppWindow> LogsUtility = std::make_shared<VortexEditor::LogsUtilityAppWindow>(label.c_str());
    Cherry::AddAppWindow(LogsUtility->GetAppWindow());
    c_LogsUtilityInstances.push_back(LogsUtility);
  }

  void Menubar(const std::shared_ptr<EditorLayer> &exampleLayer, Cherry::Application *app);

private:
  std::shared_ptr<VortexEditor::WelcomeWindow> m_WelcomeAppWindow;
  std::shared_ptr<VortexEditor::ModulesUtilityAppWindow> m_ModuleUtilityAppWindow;
  std::shared_ptr<VortexEditor::ProjectSettingsAppWindow> m_ProjectSettingsAppWindow;
  std::shared_ptr<VortexEditor::TemplatesUtilityAppWindow> m_TemplatesUtilityAppWindow;
};

static std::shared_ptr<Editor> c_Editor;

namespace VortexMaker
{
  int VortexEditor(int argc, char **argv);
}

#endif