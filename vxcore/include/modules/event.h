#include "../vortex.h"
#include "../vortex_internals.h"


#ifndef MODULE_EVENT_H
#define MODULE_EVENT_H

enum class DevFlag
{
    DEPRECIATED,
    READY, 
};


enum class HappeningState
{
    INFO, 
    WARNING, 
    ERR,
    FATAL,
};

struct ModuleInputEventHappening
{
    std::string m_trigger_name;
    HappeningState m_state;
    std::string m_log;
    std::string m_timestamp;
};

struct ModuleOutputEventHappening
{
    std::string m_trigger_name;
    HappeningState m_state;
    std::string m_log;
    std::string m_timestamp;
};


// TODO : Replace all args by json values (input and output)

/**
 * @brief Executed by other with custom parameters
 */
class ModuleInputEvent
{
public:
    ModuleInputEvent(std::function<void(ArgumentValues&, ReturnValues&)> foo, const std::string& name);
    ModuleInputEvent(std::function<void(ArgumentValues&)> foo, const std::string& name);
    ModuleInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
    ModuleInputEvent(std::function<void()> foo, const std::string& name);

    std::function<void(ArgumentValues&, ReturnValues&)> m_function;
    void trigger_happening(const std::string& trigger_name, HappeningState state, const std::string& log);

    std::string m_name;
    std::string m_description;
    std::vector<std::tuple<std::string, std::string, std::string>> m_params;
    bool m_can_callback;
    DevFlag m_devflag;
    std::vector<std::shared_ptr<ModuleInputEventHappening>> m_happenings;
};

/**
 * @brief Emmiting this even with custom parameters.
 */
class ModuleOutputEvent
{
public:
    ModuleOutputEvent(std::function<void(ArgumentValues&, ReturnValues&)> foo, const std::string& name);
    ModuleOutputEvent(std::function<void(ArgumentValues&)> foo, const std::string& name);
    ModuleOutputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
    ModuleOutputEvent(std::function<void()> foo, const std::string& name);

    std::function<void(ArgumentValues&, ReturnValues&)> m_function;
    
    void trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log);

    virtual void execute() {};

    std::string m_name;
    std::vector<std::shared_ptr<ModuleOutputEventHappening>> m_happenings;
};

#endif