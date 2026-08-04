//
//  events.hpp
//  Headers for events features on the main Vortex API
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

struct CoreInputEventHappening {
  std::string trigger_name;
  HappeningState state;
  std::string log;
  std::string timestamp;
};

class VORTEX_API CoreInputEvent {
 public:
  CoreInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  CoreInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name);
  CoreInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
  CoreInputEvent(std::function<void()> foo, const std::string &name);

  void trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log);

  std::function<void(ArgumentValues &, ReturnValues &)> function_;
  std::string name_;
  std::string description_;
  std::vector<std::tuple<std::string, std::string, std::string>> params_;
  bool can_callback_;
  DevFlag devflag_;
  std::vector<std::shared_ptr<CoreInputEventHappening>> happenings_;
};

#ifndef VORTEX_EVENTS_HPP
#define VORTEX_EVENTS_HPP

namespace vxe {
  // VORTEX_API void deploy_event(const std::shared_ptr<hArgs> &args, const std::string &event_name);
  // VORTEX_API void deploy_event(
  //     const std::shared_ptr<hArgs> &args,
  //     const std::string &event_name,
  //     void (*callback)(std::shared_ptr<hArgs> _args));

  VORTEX_API void call_output_event(
      const std::string &event_name,
      ArgumentValues &args,
      ReturnValues &ret,
      const std::string &origin = "unknow");
  VORTEX_API void call_input_event(
      const std::string &module_name,
      const std::string &event_name,
      ArgumentValues &args,
      ReturnValues &ret,
      const std::string &origin = "unknow");

  // Main core inputs events
  VORTEX_API void add_core_input_event(const CoreInputEvent &event);

  VORTEX_API void register_all_input_events();

}  // namespace vxe

#endif  // VORTEX_EVENTS_HPP