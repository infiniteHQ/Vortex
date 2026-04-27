#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef MODULE_LOAD_H
#define MODULE_LOAD_H

namespace vxe {
  VORTEX_API void LoadSystemModules(std::vector<std::shared_ptr<ModuleInterface>> &sys_modules);
  VORTEX_API void LoadEditorModules(
      const std::string &directory,
      std::vector<void *> &plugin_handles,
      std::vector<std::shared_ptr<ModuleInterface>> &plugins);
}  // namespace vxe

#endif

/*
TODO:

For funtions interface :
- Make element pools on a map (pinin, pinout, inputs, outpus, return, etc...)
- Inside each pool, put a infinite args system on a map.

Faire des fonctions pour ajouter le contenu dans le init ?
AddModuleInputEvent()
AddModuleOuputEvent()
add_module_function(ModuleFunction(with all metrics))
AddModuleLogo(logopath)
*/