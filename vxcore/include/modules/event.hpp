//
//  event.hpp
//  Headers and utilities for modules events
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex.h>
#include <vortex_internals.h>

#ifndef MODULE_EVENT_H
#define MODULE_EVENT_H

#include "helpers.hpp"

class ModuleInputEvent {
 public:
  ModuleInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  ModuleInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name);
  ModuleInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
  ModuleInputEvent(std::function<void()> foo, const std::string &name);

  void trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log);

  std::function<void(ArgumentValues &, ReturnValues &)> m_function;
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
class ModuleOutputEvent {
 public:
  ModuleOutputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  ModuleOutputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name);
  ModuleOutputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
  ModuleOutputEvent(std::function<void()> foo, const std::string &name);

  void trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log);

  virtual void execute() { };

  std::function<void(ArgumentValues &, ReturnValues &)> m_function;
  std::string m_name;
  std::vector<std::shared_ptr<ModuleOutputEventHappening>> m_happenings;
};

#endif