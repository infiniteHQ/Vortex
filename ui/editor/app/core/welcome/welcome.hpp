#pragma once

#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#include "../utils.hpp"

#ifndef WELCOME_WINDOW_H
#define WELCOME_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexEditor {

class Welcome : public std::enable_shared_from_this<Welcome> {
public:
  Welcome(const std::string &name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<Welcome> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();

  std::function<void()> m_CreateProjectCallback;
  std::function<void()> m_OpenProjectCallback;
  std::function<void()> m_SettingsCallback;
  std::function<void(const std::shared_ptr<EnvProject> &)> m_ProjectCallback;

  char ModulesSearch[512];
  std::vector<std::shared_ptr<EnvProject>> GetMostRecentProjects(
      const std::vector<std::shared_ptr<EnvProject>> &projects,
      size_t maxCount);
  std::vector<std::shared_ptr<EnvProject>> m_RecentProjects;
  std::string m_SelectedChildName;

  std::vector<std::string> vortexDists;
  std::string VortexEditorDist;
  std::string newDist;

  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
  int selected;
  float leftPaneWidth = 290.0f;
};
} // namespace VortexEditor
#endif // WELCOME_WINDOW_H