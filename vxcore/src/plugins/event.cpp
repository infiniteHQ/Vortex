//
//  event.cpp
//  Sources for plugins event system
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <plugins/event.hpp>

PluginOutputEvent::PluginOutputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : m_function(foo),
      m_name(name) {
}

PluginOutputEvent::PluginOutputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      m_name(name) {
}

PluginOutputEvent::PluginOutputEvent(std::function<void(ReturnValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      m_name(name) {
}

PluginOutputEvent::PluginOutputEvent(std::function<void()> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      m_name(name) {
}

void PluginInputEvent::trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log) {
  std::shared_ptr<PluginInputEventHappening> new_trigger = std::make_shared<PluginInputEventHappening>();
  new_trigger->m_trigger_name = trigger_name;
  new_trigger->m_state = state;
  new_trigger->m_log = log;
  new_trigger->m_timestamp = vxe::get_current_timestamp();

  if (state == HappeningState::INFO) {
    vxe::log_info("Plugins Events", log);
  } else if (state == HappeningState::WARNING) {
    vxe::log_warn("Plugins Events", log);
  } else if (state == HappeningState::ERR) {
    vxe::log_error("Plugins Events", log);
  } else if (state == HappeningState::FATAL) {
    vxe::log_fatal("Plugins Events", log);
  }

  this->m_happenings.push_back(new_trigger);
}

void PluginOutputEvent::trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log) {
  std::shared_ptr<PluginOutputEventHappening> new_trigger = std::make_shared<PluginOutputEventHappening>();
  new_trigger->m_trigger_name = trigger_name;
  new_trigger->m_state = state;
  new_trigger->m_log = log;
  new_trigger->m_timestamp = vxe::get_current_timestamp();

  if (state == HappeningState::INFO) {
    vxe::log_info("Plugins Events", log);
  } else if (state == HappeningState::WARNING) {
    vxe::log_warn("Plugins Events", log);
  } else if (state == HappeningState::ERR) {
    vxe::log_error("Plugins Events", log);
  } else if (state == HappeningState::FATAL) {
    vxe::log_fatal("Plugins Events", log);
  }

  this->m_happenings.push_back(new_trigger);
}

PluginInputEvent::PluginInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : m_function(foo),
      m_name(name) {
}

PluginInputEvent::PluginInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      m_name(name) {
}

PluginInputEvent::PluginInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      m_name(name) {
}

PluginInputEvent::PluginInputEvent(std::function<void()> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      m_name(name) {
}