#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef ABOUTPROJECT_WINDOW_H
#define ABOUTPROJECT_WINDOW_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexEditor {
// This window can be a "subappwindow" of a parent if you use the constructor
// with parent parameter.
class AboutProject : public std::enable_shared_from_this<AboutProject> {
public:
  AboutProject(const std::string &name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<AboutProject> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();

  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
};
} // namespace VortexEditor

#endif // ABOUTPROJECT_WINDOW_H