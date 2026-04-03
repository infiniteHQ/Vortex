#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef CREDITS_WINDOW_H
#define CREDITS_WINDOW_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexEditor {
// This window can be a "subappwindow" of a parent if you use the constructor
// with parent parameter.
class Credits : public std::enable_shared_from_this<Credits> {
public:
  Credits(const std::string &name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<Credits> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();

  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
};
} // namespace VortexEditor

#endif // CREDITS_WINDOW_H