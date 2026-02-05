#include <plugins/event.h>

/**
 * @brief Constructor for PluginInputEvent.
 *
 * @param foo Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param name The name of the event.
 */
PluginOutputEvent::PluginOutputEvent(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name)
    : m_function(foo), m_name(name) {}

PluginOutputEvent::PluginOutputEvent(std::function<void(ArgumentValues &)> foo,
                                     const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      m_name(name) {}

PluginOutputEvent::PluginOutputEvent(std::function<void(ReturnValues &)> foo,
                                     const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      m_name(name) {}

PluginOutputEvent::PluginOutputEvent(std::function<void()> foo,
                                     const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      m_name(name) {}

/**
 * @brief Add a new trigger happening for a current input event
 *
 * @param trigger_name Pointer to the function taking a shared_ptr to hArgs as
 * parameter.
 * @param state The name of the event.
 * @param log The name of the event.
 */
void PluginInputEvent::trigger_happening(const std::string &trigger_name,
                                         HappeningState state,
                                         const std::string &log) {
  std::shared_ptr<PluginInputEventHappening> new_trigger =
      std::make_shared<PluginInputEventHappening>();
  new_trigger->m_trigger_name = trigger_name;
  new_trigger->m_state = state;
  new_trigger->m_log = log;
  new_trigger->m_timestamp = VortexMaker::getCurrentTimeStamp();

  if (state == HappeningState::INFO) {
    VortexMaker::LogInfo("Plugins Events", log);
  } else if (state == HappeningState::WARNING) {
    VortexMaker::LogWarn("Plugins Events", log);
  } else if (state == HappeningState::ERR) {
    VortexMaker::LogError("Plugins Events", log);
  } else if (state == HappeningState::FATAL) {
    VortexMaker::LogFatal("Plugins Events", log);
  }

  this->m_happenings.push_back(new_trigger);
}

/**
 * @brief Add a new trigger happening for a current input event
 *
 * @param trigger_name Pointer to the function taking a shared_ptr to hArgs as
 * parameter.
 * @param state The name of the event.
 * @param log The name of the event.
 */
void PluginOutputEvent::trigger_happening(const std::string &trigger_name,
                                          HappeningState state,
                                          const std::string &log) {
  std::shared_ptr<PluginOutputEventHappening> new_trigger =
      std::make_shared<PluginOutputEventHappening>();
  new_trigger->m_trigger_name = trigger_name;
  new_trigger->m_state = state;
  new_trigger->m_log = log;
  new_trigger->m_timestamp = VortexMaker::getCurrentTimeStamp();

  if (state == HappeningState::INFO) {
    VortexMaker::LogInfo("Plugins Events", log);
  } else if (state == HappeningState::WARNING) {
    VortexMaker::LogWarn("Plugins Events", log);
  } else if (state == HappeningState::ERR) {
    VortexMaker::LogError("Plugins Events", log);
  } else if (state == HappeningState::FATAL) {
    VortexMaker::LogFatal("Plugins Events", log);
  }

  this->m_happenings.push_back(new_trigger);
}

PluginInputEvent::PluginInputEvent(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name)
    : m_function(foo), m_name(name) {}

PluginInputEvent::PluginInputEvent(std::function<void(ArgumentValues &)> foo,
                                   const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      m_name(name) {}

PluginInputEvent::PluginInputEvent(std::function<void(ReturnValues &)> foo,
                                   const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      m_name(name) {}

PluginInputEvent::PluginInputEvent(std::function<void()> foo,
                                   const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      m_name(name) {}