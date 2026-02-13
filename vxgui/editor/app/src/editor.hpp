#pragma once
#include "../../../../lib/cherry/cherry.hpp"

// Static windows
#include "../core/about/about.hpp"
#include "../core/modules_utility/modules_utility.hpp"
#include "../core/plugins_utility/plugins_utility.hpp"
#include "../core/project_settings/project_settings.hpp"
#include "../core/welcome/welcome.hpp"

// Instanciable windows
#include "../instances/content_browser/content_browser.hpp"
#include "../instances/doc_viewer/doc_viewer.hpp"
#include "../instances/logs_utility/logs_utility.hpp"

using namespace VortexMaker;

#ifndef EDITOR_H
#define EDITOR_H

static std::vector<std::shared_ptr<VortexEditor::ContentBrowserAppWindow>>
    c_ContentBrowserInstances;
static std::vector<std::shared_ptr<VortexEditor::LogsUtilityAppWindow>>
    c_LogsUtilityInstances;
static std::vector<std::shared_ptr<VortexEditor::DocViewerAppWindow>>
    c_DocViewerInstances;

class Editor {
public:
  Editor();

  bool GetAboutAppWindowVisibility();
  void SetAboutWindowVisibility(const bool visibility);

  void SetTemplatesUtilityVisibility(const bool &visibility);
  bool GetTemplatesUtilityVisibility();

  void SetProjectSettingsVisibility(const bool &visibility);
  bool GetProjectSettingsVisibility();

  void SetModulesUtilityVisibility(const bool &visibility);
  bool GetModulesUtilityVisibility();

  void SetPluginsUtilityVisibility(const bool &visibility);
  bool GetPluginsUtilityVisibility();

  void SetWelcomeVisibility(const bool &visibility);
  bool GetWelcomeVisibility();

  std::string SpawnContentBrowser();
  std::string SpawnDocViewer();
  void SpawnContentBrowserBottom();
  void SpawnLogsUtility();

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
  std::shared_ptr<VortexEditor::PluginsUtility> m_PluginsUtility;
  std::shared_ptr<VortexEditor::ProjectSettings> m_ProjectSettings;
  std::shared_ptr<VortexEditor::AboutVortex> m_AboutWindow;
};

static std::shared_ptr<Editor> c_Editor;

namespace VortexMaker {
VORTEX_API int VortexEditor(int argc, char **argv);
}

#endif