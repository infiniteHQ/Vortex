
#pragma once
#include "../../../../../lib/cherry/cherry.hpp"
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#ifndef VORTEX_EDITOR_DOC_VIEWER_APPWINDOW_H
#define VORTEX_EDITOR_DOC_VIEWER_APPWINDOW_H

using namespace Cherry;

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <stack>
#include <unordered_set>

namespace VortexEditor {

class DocViewerAppWindow
    : public std::enable_shared_from_this<DocViewerAppWindow> {
public:
  DocViewerAppWindow(const std::string &name);

  void OnImGuiRender();
  void menubar();

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<DocViewerAppWindow> Create(const std::string &name);
  void SetupRenderCallback();

  void Render();

private:
  VxContext *ctx;
  bool opened;
  std::shared_ptr<AppWindow> m_AppWindow;
};
} // namespace VortexEditor

#endif // LOGUTILITY_H
