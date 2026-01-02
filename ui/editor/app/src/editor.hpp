#pragma once
#include "../../../../lib/cherry/cherry.hpp"

// Static windows
#include "../core/about/about.hpp"
#include "../core/modules_utility/modules_utility.hpp"
#include "../core/welcome/welcome.hpp"
// #include "../core/templates_utility/templates_utility.hpp"
#include "../core/project_settings/project_settings.hpp"

// Instanciable windows
// #include "../instances/instance.h"
#include "../instances/content_browser/content_browser.hpp"
#include "../instances/logs_utility/logs_utility.hpp"

using namespace VortexMaker;

#ifndef EDITOR_H
#define EDITOR_H

static std::vector<std::shared_ptr<VortexEditor::ContentBrowserAppWindow>>
    c_ContentBrowserInstances;
static std::vector<std::shared_ptr<VortexEditor::LogsUtilityAppWindow>>
    c_LogsUtilityInstances;

class Editor {
public:
  Editor() {

    m_AboutWindow =
        VortexEditor::AboutVortex::Create("?loc:loc.window_names.about");
    m_AboutWindow->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(m_AboutWindow->GetAppWindow());

    m_WelcomeAppWindow =
        VortexEditor::Welcome::Create("?loc:loc.window_names.welcome");
    m_WelcomeAppWindow->GetAppWindow()->SetVisibility(true);
    Cherry::AddAppWindow(m_WelcomeAppWindow->GetAppWindow());

    m_ModulesUtility = VortexEditor::ModulesUtility::Create("Modules utily");
    m_ModulesUtility->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(m_ModulesUtility->GetAppWindow());

    m_ProjectSettings =
        VortexEditor::ProjectSettings::Create("Project settings");
    m_ProjectSettings->GetAppWindow()->SetVisibility(false);
    Cherry::AddAppWindow(m_ProjectSettings->GetAppWindow());

    /* m_TemplatesUtilityAppWindow =
         VortexEditor::TemplatesUtilityAppWindow::Create("Templates utility");
     m_TemplatesUtilityAppWindow->GetAppWindow()->SetVisibility(false);
     Cherry::AddAppWindow(m_TemplatesUtilityAppWindow->GetAppWindow());*/
  };

  bool GetAboutAppWindowVisibility() {
    return m_AboutWindow->GetAppWindow()->m_Visible;
  }

  void SetAboutWindowVisibility(const bool &visibility) {
    m_AboutWindow->GetAppWindow()->SetVisibility(visibility);
    if (visibility) {
      Cherry::ApplicationSpecification spec;
      spec.SetName("About Vortex");
      spec.SetUniqueAppWindowName("About Vortex");

      spec.MinHeight = 100;
      spec.MinWidth = 200;
      spec.Height = 450;
      spec.DisableLogo = true;
      spec.DisableResize = true;
      spec.Width = 750;
      spec.CustomTitlebar = true;
      spec.DisableWindowManagerTitleBar = true;
      spec.WindowOnlyClosable = true;
      spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
      spec.UniqueAppWindowName = m_AboutWindow->GetAppWindow()->m_Name;

      spec.UsingCloseCallback = true;
      spec.CloseCallback = [this]() {
        Cherry::DeleteAppWindow(m_AboutWindow->GetAppWindow());

        // Recreate a new sleepy instance
        m_AboutWindow =
            VortexEditor::AboutVortex::Create("?loc:loc.window_names.about");
        m_AboutWindow->GetAppWindow()->SetVisibility(false);
        Cherry::AddAppWindow(m_AboutWindow->GetAppWindow());
      };

      spec.MenubarCallback = []() {};
      spec.WindowSaves = false;
      m_AboutWindow->GetAppWindow()->AttachOnNewWindow(spec);
    }
  }

  void SetTemplatesUtilityVisibility(const bool &visibility) {
    /*m_TemplatesUtilityAppWindow->GetAppWindow()->SetVisibility(visibility);*/
  }

  bool GetTemplatesUtilityVisibility() {
    /*return m_TemplatesUtilityAppWindow->GetAppWindow()->m_Visible;*/
    return false;
  }

  void SetProjectSettingsVisibility(const bool &visibility) {
    m_ProjectSettings->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetProjectSettingsVisibility() {
    return m_ProjectSettings->GetAppWindow()->m_Visible;
  }

  void SetModulesUtilityVisibility(const bool &visibility) {
    m_ModulesUtility->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetModulesUtilityVisibility() {
    return m_ModulesUtility->GetAppWindow()->m_Visible;
  }

  void SetWelcomeVisibility(const bool &visibility) {
    // m_WelcomeAppWindow->GetAppWindow()->SetVisibility(visibility);
  }

  bool GetWelcomeVisibility() {
    // return m_WelcomeAppWindow->GetAppWindow()->m_Visible;
    return false;
  }

  void SpawnContentBrowser() {
    std::string label = "Content Browser ####Content Browser-" +
                        std::to_string(c_ContentBrowserInstances.size() + 1);
    std::shared_ptr<VortexEditor::ContentBrowserAppWindow> ContentBrowser =
        VortexEditor::ContentBrowserAppWindow::Create(
            label.c_str(),
            VortexMaker::GetCurrentContext()->projectDataPath.string());

    ContentBrowser->m_CutPathsCallback = VortexMaker::Cut;
    ContentBrowser->m_CopyPathsCallback = VortexMaker::Copy;
    ContentBrowser->m_PastePathsCallback = VortexMaker::PasteAllSelections;
    ContentBrowser->m_DeletePathCallback = VortexMaker::DeletePath;
    Cherry::AddAppWindow(ContentBrowser->GetAppWindow());
    c_ContentBrowserInstances.push_back(ContentBrowser);
  }

  void SpawnLogsUtility() {
    std::string label = "Logs utility ####Logs utility-" +
                        std::to_string(c_LogsUtilityInstances.size() + 1);
    std::shared_ptr<VortexEditor::LogsUtilityAppWindow> LogsUtility =
        VortexEditor::LogsUtilityAppWindow::Create(label.c_str());
    Cherry::AddAppWindow(LogsUtility->GetAppWindow());
    c_LogsUtilityInstances.push_back(LogsUtility);
  }

  void Menubar(Cherry::Application *app);

  bool ShowContentBrowser = true;
  bool ShowProjectViewer = false;
  bool ShowModulesManager = false;
  bool ShowTemplateManager = false;
  bool ShowProjectSettings = false;
  bool ShowLogUtility = false;

private:
  std::shared_ptr<VortexEditor::Welcome> m_WelcomeAppWindow;
  std::shared_ptr<VortexEditor::ModulesUtility> m_ModulesUtility;
  std::shared_ptr<VortexEditor::ProjectSettings> m_ProjectSettings;
  std::shared_ptr<VortexEditor::AboutVortex> m_AboutWindow;
  // std::shared_ptr<VortexEditor::TemplatesUtilityAppWindow>
  //  m_TemplatesUtilityAppWindow;
};

static std::shared_ptr<Editor> c_Editor;

namespace VortexMaker {
int VortexEditor(int argc, char **argv);
}

#endif