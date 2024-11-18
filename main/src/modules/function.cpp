#include "../../include/modules/function.h"

/**
 * @brief Constructor for ModuleFunction.
 *
 * @param foo Pointer to the function.
 * @param name The name of the function.
 */
ModuleFunction::ModuleFunction(std::function<void(const VortexMaker::Values&)> foo, const std::string& name)
    : m_function(foo), m_name(name) {}

ModuleFunction::ModuleFunction(std::function<void()> foo, const std::string& name)
    : m_function([foo](const VortexMaker::Values&) { foo(); }), m_name(name) {}
