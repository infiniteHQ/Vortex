#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"


#ifndef MODULE_INSTALL_H
#define MODULE_INSTALL_H

namespace VortexMaker {
    VORTEX_API void InstallModule(const std::string &directory);
}

#endif