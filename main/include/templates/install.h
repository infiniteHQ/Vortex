#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef TEMPLATE_INSTALL_H
#define TEMPLATE_INSTALL_H

namespace VortexMaker {
    VORTEX_API void InstallTemplateOnSystem(const std::string &directory); // From anywhere to system
    VORTEX_API void InstallTemplate(const std::string &name); // From system to project
}

#endif // TEMPLATE_INSTALL_H