#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#include "../utils.hpp"

#ifndef PROJECT_SETTINGS_WINDOW_H
#define PROJECT_SETTINGS_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexEditor {
// This window can be a "subappwindow" of a parent if you use the constructor
// with parent parameter.

struct ProjectSettingsSave {
  char name[128] = "unknow";
  char author[128] = "unknow";
  char version[128] = "unknow";
  char description[128] = "unknow";
  char type[128] = "unknow";
  bool include_system_templates;
};

struct ProjectSettingsChild {
  std::function<void()> RenderCallback;
  std::string LogoPath;
  std::string Name;
  ProjectSettingsChild(
      const std::string &name,
      const std::function<void()> &rendercallback = []() {},
      const std::string &logopath = "undefined")
      : Name(name), RenderCallback(rendercallback), LogoPath(logopath) {};
};

class ProjectSettings : public std::enable_shared_from_this<ProjectSettings> {
public:
  ProjectSettings(const std::string &name);

  void AddChild(const ProjectSettingsChild &child);
  void RemoveChild(const std::string &child_name);
  ProjectSettingsChild *GetChild(const std::string &child_name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<ProjectSettings> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();
  void ModulesRender();

  void RefreshProjectInformations();
  void UpdateProjectInformations();
  void Refresh();
  void Update();

  void RefreshProjectThemes();

  std::vector<ProjectSettingsChild> m_Childs;

  std::shared_ptr<ProjectSettingsSave> current_save;

  std::string v_ProjectNameInitial;
  std::string v_ProjectDescriptionInitial;
  std::string v_ProjectVersionInitial;
  std::string v_ProjectThumbnailPathInitial;
  std::string v_ProjectTypeInitial;
  std::string v_ProjectTagsInitial;
  std::string v_ProjectAuthorInitial;
  std::string v_ProjectWebsiteInitial;
  std::string v_ProjectSupportContactInitial;
  std::string v_ProjectCopyrightFileInitial;
  std::string v_ProjectLicenseFileInitial;
  std::string v_ProjectReadmeFileInitial;
  std::string v_ProjectRequirementsFileInitial;
  std::string v_ProjectCodeOfConductFileInitial;
  std::string v_ProjectSecurityFileInitial;
  std::string v_ProjectRootContentPathInitial;

  std::string v_ProjectName;
  std::string v_ProjectDescription;
  std::string v_ProjectVersion;
  std::string v_ProjectThumbnailPath;
  std::string v_ProjectType;
  std::string v_ProjectTags;
  std::string v_ProjectAuthor;
  std::string v_ProjectWebsite;
  std::string v_ProjectSupportContact;
  std::string v_ProjectCopyrightFile;
  std::string v_ProjectLicenseFile;
  std::string v_ProjectReadmeFile;
  std::string v_ProjectRequirementsFile;
  std::string v_ProjectCodeOfConductFile;
  std::string v_ProjectSecurityFile;
  std::string v_ProjectRootContentPath;

  std::string v_NewThemeName;

  std::vector<std::string> m_AvailableThemes;

  std::string m_NewProjectPath;
  std::string m_NewModulesPath;
  std::string m_NewPluginsPath;
  std::string m_NewTemplatesPath;
  std::string m_NewContentsPath;
  std::string m_NewVortexVersionPath;

  std::vector<std::string> m_ProjectPoolsPaths;
  std::vector<std::string> m_ModulesPoolsPaths;
  std::vector<std::string> m_PluginsPoolsPaths;
  std::vector<std::string> m_TemplatesPoolsPaths;
  std::vector<std::string> m_ContentsPoolsPaths;
  std::vector<std::string> m_VortexVersionPoolsPaths;

  std::function<void()> m_CreateProjectCallback;
  std::function<void()> m_OpenProjectCallback;
  std::function<void()> m_SettingsCallback;
  std::function<void(const std::shared_ptr<EnvProject> &)> m_ProjectCallback;

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

#endif // PROJECT_SETTINGS_WINDOW_H