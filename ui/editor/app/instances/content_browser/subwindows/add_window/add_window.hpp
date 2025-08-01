#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef CONTENT_BROWSER_ADD_WINDOW_H
#define CONTENT_BROWSER_ADD_WINDOW_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexEditor {
// This window can be a "subappwindow" of a parent if you use the constructor
// with parent parameter.
class ContentBrowserAddWindow
    : public std::enable_shared_from_this<ContentBrowserAddWindow> {
public:
  ContentBrowserAddWindow(const std::string &name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<ContentBrowserAddWindow>
  Create(const std::string &name);
  void SetupRenderCallback();
  void Render();

  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
};
} // namespace VortexEditor

#endif // CONTENT_BROWSER_ADD_WINDOW_H