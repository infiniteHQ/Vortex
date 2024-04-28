#include "../vortex.h"
#include "../vortex_internals.h"


#ifndef MODULE_EVENT_H
#define MODULE_EVENT_H

/**
 * @brief Executed by other with custom parameters
 */
class ModuleInputEvent
{
public:
    ModuleInputEvent(void(*foo)(const std::shared_ptr<hArgs>& args), const std::string& name);
    
    virtual void execute(){};

    void(*m_foo)(const std::shared_ptr<hArgs>& args);
    std::string m_name;
};

/**
 * @brief Emmiting this even with custom parameters.
 */
class ModuleOutputEvent
{
public:
    ModuleOutputEvent(void(*foo)(const std::shared_ptr<hArgs>& args), const std::string& name);
    virtual void execute() {};

    void(*m_foo)(const std::shared_ptr<hArgs>& args);
    std::string m_name;
};

#endif