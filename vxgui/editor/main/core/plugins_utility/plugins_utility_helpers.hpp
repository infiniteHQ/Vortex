//
//  modules_utility_helpers.hpp
//  Helpers and structures for the modules utility.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef MODULES_UTILITY_HELPERS_HPP
#define MODULES_UTILITY_HELPERS_HPP

namespace vxe {

  enum class ShowModes { Thumbmails, List };
  enum class Pannels { Installed, Downloads, Import };

  struct ModulesUtilityChild {
    std::function<void()> render_callback;
    std::string logo_path;
    std::string name;
    ModulesUtilityChild(
        const std::string &name,
        const std::function<void()> &render_callback = []() { },
        const std::string &logo_path = "undefined")
        : name(name),
          render_callback(render_callback),
          logo_path(logo_path) { };
  };

}  // namespace vxe
#endif  // MODULEs_UTILITY_HELPERS_HPP