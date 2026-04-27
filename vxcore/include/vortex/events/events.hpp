#pragma once
#include <vortex.h>

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
}  // namespace vxe

#endif  // VORTEX_EVENTS_HPP