#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef MODULE_INSTALL_H
#define MODULE_INSTALL_H

namespace vxe {
  VORTEX_API std::shared_ptr<ModuleInterface> FindModuleInDirectory(const std::string &directory);
  VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> FindModulesInDirectory(const std::string &directory);
  VORTEX_API void InstallModuleToSystem(const std::string &path);
  VORTEX_API void InstallModule(const std::string &module_name, const std::string &version, bool &restart_modules);
  VORTEX_API void AddModuleToProject(const std::string &module_name);
}  // namespace vxe

#endif