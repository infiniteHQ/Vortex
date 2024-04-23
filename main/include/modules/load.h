#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef MODULE_LOAD_H
#define MODULE_LOAD_H

namespace VortexMaker {
    VORTEX_API void LoadEditorModules(const std::string &directory, std::vector<void *> &plugin_handles, std::vector<std::shared_ptr<ModuleInterface>> &plugins);
}

#endif

/*
TODO:

For funtions interface : 
- Make element pools on a map (pinin, pinout, inputs, outpus, return, etc...)
- Inside each pool, put a infinite args system on a map.

Faire des fonctions pour ajouter le contenu dans le init ?
AddModuleInputEvent()
AddModuleOuputEvent()
AddModuleFunction(ModuleFunction(with all metrics))
AddModuleLogo(logopath)
*/