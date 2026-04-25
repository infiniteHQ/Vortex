//
//  plugins_utility_helpers.hpp
//  Helpers and structures for the plugins utility.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef PLUGINS_UTILITY_HELPERS_HPP
#define PLUGINS_UTILITY_HELPERS_HPP

namespace vxe {

  enum class PluginsUtilityShowModes { Thumbmails, List };
  enum class PluginsUtilityPannels { Installed, Downloads, Import };

  struct PluginsUtilityChild {
    std::function<void()> render_callback;
    std::string logo_path;
    std::string name;
    PluginsUtilityChild(
        const std::string &name,
        const std::function<void()> &render_callback = []() { },
        const std::string &logo_path = "undefined")
        : name(name),
          render_callback(render_callback),
          logo_path(logo_path) { };
  };

}  // namespace vxe
#endif  // MODULEs_UTILITY_HELPERS_HPP