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
    : function_(foo),
      name_(name) {
}

PluginOutputEvent::PluginOutputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : function_([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      name_(name) {
}

PluginOutputEvent::PluginOutputEvent(std::function<void(ReturnValues &)> foo, const std::string &name)
    : function_([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      name_(name) {
}

PluginOutputEvent::PluginOutputEvent(std::function<void()> foo, const std::string &name)
    : function_([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      name_(name) {
}

void PluginInputEvent::trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log) {
  std::shared_ptr<PluginInputEventHappening> new_trigger = std::make_shared<PluginInputEventHappening>();
  new_trigger->trigger_name_ = trigger_name;
  new_trigger->state_ = state;
  new_trigger->log_ = log;
  new_trigger->timestamp_ = vxe::get_current_timestamp();

  if (state == HappeningState::INFO) {
    vxe::log_info("Plugins Events", log);
  } else if (state == HappeningState::WARNING) {
    vxe::log_warn("Plugins Events", log);
  } else if (state == HappeningState::ERR) {
    vxe::log_error("Plugins Events", log);
  } else if (state == HappeningState::FATAL) {
    vxe::log_fatal("Plugins Events", log);
  }

  this->happenings_.push_back(new_trigger);
}

void PluginOutputEvent::trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log) {
  std::shared_ptr<PluginOutputEventHappening> new_trigger = std::make_shared<PluginOutputEventHappening>();
  new_trigger->trigger_name_ = trigger_name;
  new_trigger->state_ = state;
  new_trigger->log_ = log;
  new_trigger->timestamp_ = vxe::get_current_timestamp();

  if (state == HappeningState::INFO) {
    vxe::log_info("Plugins Events", log);
  } else if (state == HappeningState::WARNING) {
    vxe::log_warn("Plugins Events", log);
  } else if (state == HappeningState::ERR) {
    vxe::log_error("Plugins Events", log);
  } else if (state == HappeningState::FATAL) {
    vxe::log_fatal("Plugins Events", log);
  }

  this->happenings_.push_back(new_trigger);
}

PluginInputEvent::PluginInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : function_(foo),
      name_(name) {
}

PluginInputEvent::PluginInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : function_([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      name_(name) {
}

PluginInputEvent::PluginInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name)
    : function_([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      name_(name) {
}

PluginInputEvent::PluginInputEvent(std::function<void()> foo, const std::string &name)
    : function_([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      name_(name) {
}