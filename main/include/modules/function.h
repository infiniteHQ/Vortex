#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef MODULE_FUNCTION_H
#define MODULE_FUNCTION_H


class ModuleFunction
{
public:
    ModuleFunction(void(*foo)(), const std::string& name);

    virtual void execute() {};

    void(*m_foo)();
    std::string m_name;
    //std::shared_ptr<Parameters> m_params;
    std::vector<std::string> m_params_def;
};

#endif