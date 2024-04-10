#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

namespace VortexMaker {
    VORTEX_API void LoadEditorModules(const std::string &directory, std::vector<void *> &plugin_handles, std::vector<std::shared_ptr<ModuleInterface>> &plugins);
}