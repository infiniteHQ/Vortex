
#pragma once
#include "../../../vxcore/include/vortex.h"

// Static windows
#include "./core/about/about.hpp"
#include "./core/about_project/about_project.hpp"
#include "./core/credits/credits.hpp"
#include "./core/modules_utility/modules_utility.hpp"
#include "./core/plugins_utility/plugins_utility.hpp"
#include "./core/project_settings/project_settings.hpp"
#include "./core/welcome/welcome.hpp"

// Instanciable windows
#include "./instances/content_browser/content_browser.hpp"
#include "./instances/doc_viewer/doc_viewer.hpp"
#include "./instances/logs_utility/logs_utility.hpp"

using namespace vxe;

#ifndef EDITOR_H
#define EDITOR_H

static std::vector<std::shared_ptr<vxe::ContentBrowser>> c_ContentBrowserInstances;
static std::vector<std::shared_ptr<vxe::LogsUtility>> c_LogsUtilityInstances;
static std::vector<std::shared_ptr<vxe::DocViewerAppWindow>> c_DocViewerInstances;

class Editor {
 public:
  Editor();

  bool GetCreditsVisibility();
  void SetCreditsVisibility(const bool visibility);

  bool GetAboutAppWindowVisibility();
  void SetAboutWindowVisibility(const bool visibility);

  bool GetAboutProjectAppWindowVisibility();
  void SetAboutProjectWindowVisibility(const bool visibility);

  void SetTemplatesUtilityVisibility(const bool &visibility);
  bool GetTemplatesUtilityVisibility();

  void SetProjectSettingsVisibility(const bool &visibility, const std::string &tab);
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
  void ToggleProjectSettings();
  void Menubar(Cherry::Application *app);

  bool ShowContentBrowser = true;
  bool ShowProjectViewer = false;
  bool ShowModulesManager = false;
  bool ShowTemplateManager = false;
  bool ShowProjectSettings = false;
  bool ShowLogUtility = false;

 private:
  std::shared_ptr<vxe::Welcome> m_WelcomeAppWindow;
  std::shared_ptr<vxe::ModulesUtility> m_ModulesUtility;
  std::shared_ptr<vxe::PluginsUtility> m_PluginsUtility;
  std::shared_ptr<vxe::ProjectSettings> m_ProjectSettings;
  std::shared_ptr<vxe::AboutVortex> m_AboutWindow;
  std::shared_ptr<vxe::AboutProject> m_AboutProjectWindow;
  std::shared_ptr<vxe::Credits> m_CreditsWindow;
};

static std::shared_ptr<Editor> c_Editor;

namespace vxe {
  VORTEX_API int VortexEditor(int argc, char **argv);
}

#endif