
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

enum class ShowMode { Advanced, Simple, Block };

struct LogUtilityChild {
  std::function<void()> m_Child;
  std::string m_Name;
  bool m_Disabled = true;
  float m_DefaultSize = 0.0f;
  float m_Size = 0.0f;
  ImVec4 m_BackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

  void Enable() { m_Disabled = false; }

  void Disable() { m_Disabled = true; }

  LogUtilityChild(const std::string &name, const std::function<void()> &child,
                  float defaultSize = 0.0f)
      : m_Name(name), m_Child(child), m_DefaultSize(defaultSize),
        m_Size(defaultSize) {}
  LogUtilityChild() {};
};

class LogsUtilityAppWindow
    : public std::enable_shared_from_this<LogsUtilityAppWindow> {
public:
  LogsUtilityAppWindow(const std::string &name);

  void OnImGuiRender();
  void menubar();

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<LogsUtilityAppWindow> Create(const std::string &name);
  void SetupRenderCallback();

  void AddChild(const LogUtilityChild &child);

  void Render();
  void RenderContentBar();
  void RenderFiltersBar();

private:
  VxContext *ctx;
  bool opened;
  std::string m_CmdInputValue;
  std::string m_LogSearchValue;
  std::shared_ptr<AppWindow> m_AppWindow;
  bool m_ShowFilterPannel = false;
  bool m_PreviousFilterPannelState = false;
  bool m_ChildSizesInitialized = false;

  ShowMode m_ShowMode = ShowMode::Simple;
  bool m_WillShowFilterPannel = false;
  bool m_UseWarningFilter = false;
  bool m_UseErrorFilter = false;
  bool m_UseFatalFilter = false;
  bool m_UseInfoFilter = false;
  bool m_SearchBar = false;

  std::map<std::string, bool> m_TopicsFilterStates;

  void ActivateTopicFilter(const std::string &topic) {

    m_TopicsFilterStates[topic] = true;
  }

  std::vector<LogUtilityChild> m_Childs;
};
} // namespace VortexEditor

#endif // LOGUTILITY_H
