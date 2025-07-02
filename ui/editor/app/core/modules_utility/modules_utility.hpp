#pragma once

#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#include "../utils.hpp"

#ifndef MODULES_UTILITY_WINDOW_H
#define MODULES_UTILITY_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexEditor {

struct ModulesUtilityChild {
  std::function<void()> RenderCallback;
  std::string LogoPath;
  std::string Name;
  ModulesUtilityChild(
      const std::string &name,
      const std::function<void()> &rendercallback = []() {},
      const std::string &logopath = "undefined")
      : Name(name), RenderCallback(rendercallback), LogoPath(logopath) {};
};

class ModulesUtility : public std::enable_shared_from_this<ModulesUtility> {
public:
  ModulesUtility(const std::string &name);

  void AddChild(const ModulesUtilityChild &child);
  void RemoveChild(const std::string &child_name);
  ModulesUtilityChild *GetChild(const std::string &child_name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<ModulesUtility> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();
  void ModulesRender();

  std::vector<ModulesUtilityChild> m_Childs;

  bool ModuleCard(const std::shared_ptr<ModuleInterface> &module,
                  const std::string &name, const std::string &path,
                  const std::string &description, const std::string &size,
                  bool selected, const std::string &logo,
                  ImU32 bgColor = IM_COL32(100, 100, 100, 255),
                  ImU32 borderColor = IM_COL32(150, 150, 150, 255),
                  ImU32 lineColor = IM_COL32(255, 255, 0, 255),
                  float maxTextWidth = 100.0f, float borderRadius = 5.0f);

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
#endif // MODULEs_UTILITY_WINDOW_H