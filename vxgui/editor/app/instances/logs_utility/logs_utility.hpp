
#pragma once
#include "../../../../../lib/cherry/cherry.hpp"
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
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

struct ConsoleEntry {
  Cherry::Log::Level level;
  std::string message;
  int count;
};

static std::vector<ConsoleEntry> s_Entries;
static char s_InputBuffer[256] = "";
static bool s_ScrollToBottom = false;
static std::vector<std::string> s_CommandHistory;
static int s_HistoryPos = -1;

static int s_SelectionIdx = -1;
struct CommandDef {
  std::string name;
  std::string params;
  std::string description;
  std::string snippet;
};

static std::vector<CommandDef> s_CommandDefs = {
    // Logs & Debug
    {"Log", "text", "Simple message to the logs", "Log(\"<cursor>\")"},
    {"LogWarn", "text", "Warning message to the logs", "LogWarn(\"<cursor>\")"},
    {"LogError", "text", "Error message to the logs", "LogError(\"<cursor>\")"},
    {"LogFatal", "text", "Fatal message to the logs", "LogFatal(\"<cursor>\")"},

    // Modules/Plugins events
    {"CallOutputEvent", "text", "Call a global output event",
     "CallOutputEvent(\"<cursor>\", {})"},
    {"CallInputEvent", "text", "Call an input event on a module",
     "CallInputEvent(\"<cursor>\", \"eventName\", {})"},

    // Cross platform
    {"IsLinux", "text", "Returns true if running on Linux", "IsLinux()"},
    {"IsNotLinux", "text", "Returns true if not running on Linux",
     "IsNotLinux()"},
    {"IsWindows", "text", "Returns true if running on Windows", "IsWindows()"},
    {"IsNotWindows", "text", "Returns true if not running on Windows",
     "IsNotWindows()"},
    {"IsMacOS", "text", "Returns true if running on macOS", "IsMacOS()"},
    {"IsNotMacOS", "text", "Returns true if not running on macOS",
     "IsNotMacOS()"},

    // Information functions
    {"GetBuildDate", "text", "Returns the build date of Vortex",
     "GetBuildDate()"},
    {"GetGitCommit", "text", "Returns the git commit hash of this build",
     "GetGitCommit()"},
    {"GetVortexEditorDist", "text", "Returns the editor distribution name",
     "GetVortexEditorDist()"},
    {"GetVortexBuildID", "text", "Returns the unique build ID",
     "GetVortexBuildID()"},
    {"GetVortexBuildName", "text", "Returns the readable build name",
     "GetVortexBuildName()"},
    {"GetVortexEditorHash", "text", "Returns the editor hash for this build",
     "GetVortexEditorHash()"},
};
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
  void AddLogEntry(Cherry::Log::Level level, const std::string &msg);
  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<LogsUtilityAppWindow> Create(const std::string &name);
  void SetupRenderCallback();

  void AddChild(const LogUtilityChild &child);

  void Render();
  void RenderContentBar();
  void RenderFiltersBar();

  void SendCommand(const std::string &cmd);

  bool HasCommonSubsequence(const std::string &a, const std::string &b) {
    int j = 0;
    for (int i = 0; i < a.size() && j < b.size(); i++) {
      if (a[i] == b[j])
        j++;
    }
    return j == b.size();
  }

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
  bool m_ConsoleFont = true;
  bool m_SearchBar = false;

  std::map<std::string, bool> m_TopicsFilterStates;

  void ActivateTopicFilter(const std::string &topic) {

    m_TopicsFilterStates[topic] = true;
  }

  std::vector<LogUtilityChild> m_Childs;
};
} // namespace VortexEditor

#endif // LOGUTILITY_H
