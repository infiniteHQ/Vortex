#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_DELETE_H
#define PLUGIN_DELETE_H

namespace VortexMaker {
VORTEX_API void DeleteSystemPlugin(const std::string &name,
                                   const std::string &version);
}

#endif