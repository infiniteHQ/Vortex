#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef MODULES_UTILITY_HELPERS_HPP
#define MODULES_UTILITY_HELPERS_HPP

namespace vxe {

  struct ModulesUtilityChild {
    std::function<void()> RenderCallback;
    std::string LogoPath;
    std::string Name;
    ModulesUtilityChild(
        const std::string &name,
        const std::function<void()> &rendercallback = []() { },
        const std::string &logopath = "undefined")
        : Name(name),
          RenderCallback(rendercallback),
          LogoPath(logopath) { };
  };

}  // namespace vxe
#endif  // MODULEs_UTILITY_HELPERS_HPP