//
//  helpers.hpp
//  Structures and helpers for plugins
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex.h>
#include <vortex_internals.h>

#ifndef PLUGIN_HELPERS_H
#define PLUGIN_HELPERS_H

// Forward declarations
enum class HappeningState;
enum class DevFlag;
enum class HandlerItemType;

struct PluginInputEventHappening {
  std::string trigger_name_;
  HappeningState state_;
  std::string log_;
  std::string timestamp_;
};

struct PluginOutputEventHappening {
  std::string trigger_name_;
  HappeningState state_;
  std::string log_;
  std::string timestamp_;
};

#endif