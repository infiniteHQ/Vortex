#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef MODULES_DETAILS_APPWINDOW_H
#define MODULES_DETAILS_APPWINDOW_H

using namespace VortexMaker;

class ModuleDetails : public std::enable_shared_from_this<ModuleDetails> {
public:
  ModuleDetails(const std::string &name,
                const std::shared_ptr<ModuleInterface> &module);

  void close();

  void menubar();

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<ModuleDetails>
  Create(const std::string &name,
         const std::shared_ptr<ModuleInterface> &module);
  void SetupRenderCallback();
  void Render();

  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
  void Refresh();
  void Save();

  void UI_ContentWindow();
  void UI_TasksEditor();
  void UI_ParametersWindow();
  void UI_CurrentToolchainPreview();
  void UI_SnapshotUtility();
  void UI_ManualBuild();

  bool opened;
  bool show_UI_ContentWindow = false;
  bool show_UI_ParametersWindow = false;
  bool show_UI_TasksEditor = false;
  bool show_UI_CurrentToolchainPreview = false;
  bool show_UI_SnapshotUtility = false;
  bool show_UI_ManualBuild = false;

  std::string name;
  VxContext *m_ctx;
  std::shared_ptr<ModuleInterface> m_module;
};

#endif // MODULES_DETAILS_APPWINDOW_H