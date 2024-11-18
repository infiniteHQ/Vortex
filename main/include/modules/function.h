#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef MODULE_FUNCTION_H
#define MODULE_FUNCTION_H

// TODO : Replace all args by json values (params like return)

/*
Functions models :

   Foo        Params           Return
Function [JSONPARAMETERS]   [JSONRETURN]
Function [void]             [JSONRETURN]
Function [JSONPARAMETERS]   [void]
Function [void]             [void]
*/

    /**
     * @brief Scopes of a ModuleFunction. If PUBLIC, the function can be called by every other members of the project (plugins,
     * modules, etc...). If PRIVATE, the function can only be called by itself. If RESTRICTED, the function can be called by linked 
     * members (child plugins/modules, optionnal extentions of the module/plugin)
     */
enum class ModuleFunctionScope
{
    PRIVATE,
    PUBLIC,
    RESTRICTED
};

class ModuleFunction
{
public:
    ModuleFunction(std::function<void(const VortexMaker::Values&)> foo, const std::string& name);
    ModuleFunction(std::function<void()> foo, const std::string& name);

    virtual void execute() {};

    std::function<void(const VortexMaker::Values&)> m_function;
    VortexMaker::Values m_return_values;
    ModuleFunctionScope m_scope;

    std::string m_name;
    std::string m_description;
    bool        m_can_callback;
    std::shared_ptr<hArgs> m_args;
    std::vector<std::tuple<std::string,std::string,std::string>> m_params_def;
};

#endif