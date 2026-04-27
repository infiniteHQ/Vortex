//
//  event.cpp
//  Sources for modules events features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../include/modules/event.hpp"

#include "../../include/vortex_internals.h"

ModuleOutputEvent::ModuleOutputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : m_function(foo),
      name_(name) {
}

ModuleOutputEvent::ModuleOutputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      name_(name) {
}

ModuleOutputEvent::ModuleOutputEvent(std::function<void(ReturnValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      name_(name) {
}

ModuleOutputEvent::ModuleOutputEvent(std::function<void()> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      name_(name) {
}

/**
 * @brief Add a new trigger happening for a current input event
 *
 * @param trigger_name Pointer to the function taking a shared_ptr to hArgs as
 * parameter.
 * @param state The name of the event.
 * @param log The name of the event.
 */
void ModuleInputEvent::trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log) {
  std::shared_ptr<ModuleInputEventHappening> new_trigger = std::make_shared<ModuleInputEventHappening>();
  new_trigger->timestamp = trigger_name;
  new_trigger->state = state;
  new_trigger->log = log;
  new_trigger->timestamp = vxe::get_current_timestamp();

  if (state == HappeningState::INFO) {
    vxe::log_info("Modules Events", log);
  } else if (state == HappeningState::WARNING) {
    vxe::log_warn("Modules Events", log);
  } else if (state == HappeningState::ERR) {
    vxe::log_error("Modules Events", log);
  } else if (state == HappeningState::FATAL) {
    vxe::log_fatal("Modules Events", log);
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
void ModuleOutputEvent::trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log) {
  std::shared_ptr<ModuleOutputEventHappening> new_trigger = std::make_shared<ModuleOutputEventHappening>();
  new_trigger->trigger_name = trigger_name;
  new_trigger->state = state;
  new_trigger->log = log;
  new_trigger->timestamp = vxe::get_current_timestamp();

  if (state == HappeningState::INFO) {
    vxe::log_info("Modules Events", log);
  } else if (state == HappeningState::WARNING) {
    vxe::log_warn("Modules Events", log);
  } else if (state == HappeningState::ERR) {
    vxe::log_error("Modules Events", log);
  } else if (state == HappeningState::FATAL) {
    vxe::log_fatal("Modules Events", log);
  }

  this->m_happenings.push_back(new_trigger);
}

ModuleInputEvent::ModuleInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : m_function(foo),
      name_(name) {
}

ModuleInputEvent::ModuleInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      name_(name) {
}

ModuleInputEvent::ModuleInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      name_(name) {
}

ModuleInputEvent::ModuleInputEvent(std::function<void()> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      name_(name) {
}