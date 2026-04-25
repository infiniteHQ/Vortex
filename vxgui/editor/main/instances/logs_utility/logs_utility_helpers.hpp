//
//  logs_utility_helpers.hpp
//  Helpers strucutres and enums for the logs utility
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef VORTEX_EDITOR_LOGS_UTILITY_HELPERS_HPP
#define VORTEX_EDITOR_LOGS_UTILITY_HELPERS_HPP

namespace vxe {

  enum class ShowMode { Advanced, Simple, Block };

  struct ConsoleEntry {
    Cherry::Log::Level level;
    std::string message;
    int count;
  };

  struct CommandDef {
    std::string name;
    std::string params;
    std::string description;
    std::string snippet;
  };

  struct LogUtilityChild {
    std::function<void()> m_Child;
    std::string m_Name;
    bool m_Disabled = true;
    float m_DefaultSize = 0.0f;
    float m_Size = 0.0f;
    ImVec4 m_BackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    void Enable() {
      m_Disabled = false;
    }

    void Disable() {
      m_Disabled = true;
    }

    LogUtilityChild(const std::string &name, const std::function<void()> &child, float defaultSize = 0.0f)
        : m_Name(name),
          m_Child(child),
          m_DefaultSize(defaultSize),
          m_Size(defaultSize) {
    }
    LogUtilityChild() { };
  };

  static std::vector<CommandDef> COMMAND_DEFINITIONS = {
    // Logs & Debug
    { "Log", "text", "Simple message to the logs", "Log(\"<cursor>\")" },
    { "LogWarn", "text", "Warning message to the logs", "LogWarn(\"<cursor>\")" },
    { "LogError", "text", "Error message to the logs", "LogError(\"<cursor>\")" },
    { "LogFatal", "text", "Fatal message to the logs", "LogFatal(\"<cursor>\")" },

    // Modules/Plugins events
    { "CallOutputEvent", "text", "Call a global output event", "CallOutputEvent(\"<cursor>\", {})" },
    { "CallInputEvent", "text", "Call an input event on a module", "CallInputEvent(\"<cursor>\", \"eventName\", {})" },

    // Cross platform
    { "IsLinux", "text", "Returns true if running on Linux", "IsLinux()" },
    { "IsNotLinux", "text", "Returns true if not running on Linux", "IsNotLinux()" },
    { "IsWindows", "text", "Returns true if running on Windows", "IsWindows()" },
    { "IsNotWindows", "text", "Returns true if not running on Windows", "IsNotWindows()" },
    { "IsMacOS", "text", "Returns true if running on macOS", "IsMacOS()" },
    { "IsNotMacOS", "text", "Returns true if not running on macOS", "IsNotMacOS()" },

    // Information functions
    { "GetBuildDate", "text", "Returns the build date of Vortex", "GetBuildDate()" },
    { "GetGitCommit", "text", "Returns the git commit hash of this build", "GetGitCommit()" },
    { "GetVortexEditorDist", "text", "Returns the editor distribution name", "GetVortexEditorDist()" },
    { "GetVortexBuildID", "text", "Returns the unique build ID", "GetVortexBuildID()" },
    { "GetVortexBuildName", "text", "Returns the readable build name", "GetVortexBuildName()" },
    { "GetVortexEditorHash", "text", "Returns the editor hash for this build", "GetVortexEditorHash()" },
  };

}  // namespace vxe

#endif  // LOGUTILITY_H
