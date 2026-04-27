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
    { "log_warn", "text", "Warning message to the logs", "log_warn(\"<cursor>\")" },
    { "log_error", "text", "Error message to the logs", "log_error(\"<cursor>\")" },
    { "log_fatal", "text", "Fatal message to the logs", "log_fatal(\"<cursor>\")" },

    // Modules/Plugins events
    { "call_output_event", "text", "Call a global output event", "call_output_event(\"<cursor>\", {})" },
    { "call_input_event", "text", "Call an input event on a module", "call_input_event(\"<cursor>\", \"eventName\", {})" },

    // Cross platform
    { "is_linux", "text", "Returns true if running on Linux", "is_linux()" },
    { "is_not_linux", "text", "Returns true if not running on Linux", "is_not_linux()" },
    { "is_windows", "text", "Returns true if running on Windows", "is_windows()" },
    { "is_not_windows", "text", "Returns true if not running on Windows", "is_not_windows()" },
    { "IsMacOS", "text", "Returns true if running on macOS", "IsMacOS()" },
    { "is_not_macos", "text", "Returns true if not running on macOS", "is_not_macos()" },

    // Information functions
    { "get_build_date", "text", "Returns the build date of Vortex", "get_build_date()" },
    { "get_git_commit", "text", "Returns the git commit hash of this build", "get_git_commit()" },
    { "get_vortex_editor_dist", "text", "Returns the editor distribution name", "get_vortex_editor_dist()" },
    { "get_vortex_build_id", "text", "Returns the unique build ID", "get_vortex_build_id()" },
    { "get_vortex_build_name", "text", "Returns the readable build name", "get_vortex_build_name()" },
    { "get_vortex_editor_hash", "text", "Returns the editor hash for this build", "get_vortex_editor_hash()" },
  };

}  // namespace vxe

#endif  // LOGUTILITY_H
