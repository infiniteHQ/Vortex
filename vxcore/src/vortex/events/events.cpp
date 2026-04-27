//
//  events.cpp
//  Sources for events main features related to modules and plugins
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

void vxe::call_output_event(
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret,
    const std::string &origin) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Iterate through each EventManager in the Vortex context
  for (auto em : ctx->IO.em) {
    // Iterate through each input event in the EventManager
    for (auto output_event : em->output_events()) {
      // Check if the input event name matches the provided event name
      if (output_event->name_ == event_name) {
        // Check if the event function pointer is valid
        if (output_event->function_) {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Calling module output event \"" + output_event->name_ + "\" of module \"" + em->name() + "\" from \"" +
                  origin + "\"");

          // Call the corresponding event function with the provided arguments
          output_event->function_(args, ret);

          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Output event \"" + output_event->name_ + "\" of module \"" + em->name() + "\" called succefully from \"" +
                  origin + "\" !");
        } else {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Trying to call \"" + output_event->name_ + "\" of module \"" + em->name() + "\" from \"" + origin +
                  "\" but it not exist !");
        }
      }
    }
  }

  // Iterate through each plugins
  for (auto ep : ctx->IO.ep) {
    // Iterate through each input event in the EventManager
    for (auto output_event : ep->output_events()) {
      // Check if the input event name matches the provided event name
      if (output_event->name_ == event_name) {
        // Check if the event function pointer is valid
        if (output_event->function_) {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Calling module output event \"" + output_event->name_ + "\" of module \"" + ep->name() + "\" from \"" +
                  origin + "\"");

          // Call the corresponding event function with the provided arguments
          output_event->function_(args, ret);

          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Output event \"" + output_event->name_ + "\" of module \"" + ep->name() + "\" called succefully from \"" +
                  origin + "\" !");
        } else {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Trying to call \"" + output_event->name_ + "\" of module \"" + ep->name() + "\" from \"" + origin +
                  "\" but it not exist !");
        }
      }
    }
  }
}

void vxe::call_input_event(
    const std::string &module_name,
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret,
    const std::string &origin) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  bool event_hit = false;

  for (auto em : ctx->IO.em) {
    if (em->name() == module_name) {
      for (auto input_event : em->input_events()) {
        if (input_event->name_ == event_name) {
          if (input_event->function_) {
            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Calling module input event \"" + input_event->name_ + "\" of module \"" + em->name() + "\" from \"" +
                    origin + "\"");

            // Call the corresponding event function with the provided arguments
            input_event->function_(args, ret);

            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Input event \"" + input_event->name_ + "\" of module \"" + em->name() + "\" called succefully from \"" +
                    origin + "\" !");
          } else {
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Trying to call \"" + input_event->name_ + "\" of module \"" + em->name() + "\" from \"" + origin +
                    "\" but it not exist !");
          }

          event_hit = true;
        }
      }
    }
  }

  // Iterate through each plguins in the Vortex context
  for (auto ep : ctx->IO.ep) {
    // Check if the EventManager corresponds to the specified module
    if (ep->name() == module_name) {
      // Iterate through each input event in the EventManager
      for (auto input_event : ep->input_events()) {
        // Check if the input event name matches the provided event name
        if (input_event->name_ == event_name) {
          // Check if the event function pointer is valid
          if (input_event->function_) {
            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Calling module input event \"" + input_event->name_ + "\" of module \"" + ep->name() + "\" from \"" +
                    origin + "\"");

            // Call the corresponding event function with the provided arguments
            input_event->function_(args, ret);

            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Input event \"" + input_event->name_ + "\" of module \"" + ep->name() + "\" called succefully from \"" +
                    origin + "\" !");
          } else {
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Trying to call \"" + input_event->name_ + "\" of module \"" + ep->name() + "\" from \"" + origin +
                    "\" but it not exist !");
          }

          event_hit = true;
        }
      }
    }
  }

  if (!event_hit) {
    std::string log = "Input event not found. (target:" + module_name + " , event:" + event_name + ")";
    vxe::log_error("Events", log);
  }
}