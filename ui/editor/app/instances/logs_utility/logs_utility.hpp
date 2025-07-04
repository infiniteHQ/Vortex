
#pragma once
#include "../../../../../lib/cherry/cherry.hpp"
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"
#ifndef VORTEX_EDITOR_LOGS_UTILITY_APPWINDOW_H
#define VORTEX_EDITOR_LOGS_UTILITY_APPWINDOW_H

using namespace Cherry;

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <stack>
#include <unordered_set>

namespace VortexEditor {
class LogsUtilityAppWindow
    : public std::enable_shared_from_this<LogsUtilityAppWindow> {
public:
  LogsUtilityAppWindow(const std::string &name);

  void OnImGuiRender();
  void menubar();

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<LogsUtilityAppWindow> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();

private:
  VxContext *ctx;
  bool opened;
  std::string m_CmdInputValue;
  std::string m_LogSearchValue;
  std::shared_ptr<AppWindow> m_AppWindow;
};
} // namespace VortexEditor

#endif // LOGUTILITY_H
