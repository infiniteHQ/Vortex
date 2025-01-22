#include "../../include/modules/event.h"

/**
 * @brief Constructor for ModuleInputEvent.
 *
 * @param foo Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param name The name of the event.
 */
ModuleOutputEvent::ModuleOutputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : m_function(foo), m_name(name) {}

ModuleOutputEvent::ModuleOutputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret)
                 {
          ret = ReturnValues(); 
          foo(args); }),
      m_name(name) {}

ModuleOutputEvent::ModuleOutputEvent(std::function<void(ReturnValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret)
                 {
                     foo(ret);
                 }),
      m_name(name) {}

ModuleOutputEvent::ModuleOutputEvent(std::function<void()> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &)
                 { foo(); }),
      m_name(name) {}

/**
 * @brief Add a new trigger happening for a current input event
 *
 * @param trigger_name Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param state The name of the event.
 * @param log The name of the event.
 */
void ModuleInputEvent::trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log)
{
    std::shared_ptr<ModuleInputEventHappening> new_trigger = std::make_shared<ModuleInputEventHappening>();
    new_trigger->m_trigger_name = trigger_name;
    new_trigger->m_state = state;
    new_trigger->m_log = log;
    new_trigger->m_timestamp = VortexMaker::getCurrentTimeStamp();

    if (state == HappeningState::INFO)
    {
        VortexMaker::LogInfo("Modules Events", log);
    }
    else if (state == HappeningState::WARNING)
    {
        VortexMaker::LogWarn("Modules Events", log);
    }
    else if (state == HappeningState::ERR)
    {
        VortexMaker::LogError("Modules Events", log);
    }
    else if (state == HappeningState::FATAL)
    {
        VortexMaker::LogFatal("Modules Events", log);
    }

    this->m_happenings.push_back(new_trigger);
}


/**
 * @brief Add a new trigger happening for a current input event
 *
 * @param trigger_name Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param state The name of the event.
 * @param log The name of the event.
 */
void ModuleOutputEvent::trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log)
{
    std::shared_ptr<ModuleOutputEventHappening> new_trigger = std::make_shared<ModuleOutputEventHappening>();
    new_trigger->m_trigger_name = trigger_name;
    new_trigger->m_state = state;
    new_trigger->m_log = log;
    new_trigger->m_timestamp = VortexMaker::getCurrentTimeStamp();

    if (state == HappeningState::INFO)
    {
        VortexMaker::LogInfo("Modules Events", log);
    }
    else if (state == HappeningState::WARNING)
    {
        VortexMaker::LogWarn("Modules Events", log);
    }
    else if (state == HappeningState::ERR)
    {
        VortexMaker::LogError("Modules Events", log);
    }
    else if (state == HappeningState::FATAL)
    {
        VortexMaker::LogFatal("Modules Events", log);
    }

    this->m_happenings.push_back(new_trigger);
}

ModuleInputEvent::ModuleInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : m_function(foo), m_name(name) {}

ModuleInputEvent::ModuleInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret)
                 {
          ret = ReturnValues(); 
          foo(args); }),
      m_name(name) {}

ModuleInputEvent::ModuleInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret)
                 {
                     foo(ret);
                 }),
      m_name(name) {}

ModuleInputEvent::ModuleInputEvent(std::function<void()> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &)
                 { foo(); }),
      m_name(name) {}