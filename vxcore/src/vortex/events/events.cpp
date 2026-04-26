#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void
vxe::call_output_event(const std::string &event_name, ArgumentValues &args, ReturnValues &ret, const std::string &origin) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Iterate through each EventManager in the Vortex context
  for (auto em : ctx.IO.em) {
    // Iterate through each input event in the EventManager
    for (auto output_event : em->m_output_events) {
      // Check if the input event name matches the provided event name
      if (output_event->m_name == event_name) {
        // Check if the event function pointer is valid
        if (output_event->m_function) {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Calling module output event \"" + output_event->m_name + "\" of module \"" + em->m_name + "\" from \"" +
                  origin + "\"");

          // Call the corresponding event function with the provided arguments
          output_event->m_function(args, ret);

          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Output event \"" + output_event->m_name + "\" of module \"" + em->m_name + "\" called succefully from \"" +
                  origin + "\" !");
        } else {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Trying to call \"" + output_event->m_name + "\" of module \"" + em->m_name + "\" from \"" + origin +
                  "\" but it not exist !");
        }
      }
    }
  }

  // Iterate through each plugins
  for (auto ep : ctx.IO.ep) {
    // Iterate through each input event in the EventManager
    for (auto output_event : ep->m_output_events) {
      // Check if the input event name matches the provided event name
      if (output_event->m_name == event_name) {
        // Check if the event function pointer is valid
        if (output_event->m_function) {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Calling module output event \"" + output_event->m_name + "\" of module \"" + ep->m_name + "\" from \"" +
                  origin + "\"");

          // Call the corresponding event function with the provided arguments
          output_event->m_function(args, ret);

          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Output event \"" + output_event->m_name + "\" of module \"" + ep->m_name + "\" called succefully from \"" +
                  origin + "\" !");
        } else {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event",
              HappeningState::INFO,
              "Trying to call \"" + output_event->m_name + "\" of module \"" + ep->m_name + "\" from \"" + origin +
                  "\" but it not exist !");
        }
      }
    }
  }
}

VORTEX_API void vxe::call_input_event(
    const std::string &module_name,
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret,
    const std::string &origin) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  bool event_hit = false;

  // Iterate through each EventManager in the Vortex context
  for (auto em : ctx.IO.em) {
    // Check if the EventManager corresponds to the specified module
    if (em->m_name == module_name) {
      // Iterate through each input event in the EventManager
      for (auto input_event : em->m_input_events) {
        // Check if the input event name matches the provided event name
        if (input_event->m_name == event_name) {
          // Check if the event function pointer is valid
          if (input_event->m_function) {
            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Calling module input event \"" + input_event->m_name + "\" of module \"" + em->m_name + "\" from \"" +
                    origin + "\"");

            // Call the corresponding event function with the provided arguments
            input_event->m_function(args, ret);

            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Input event \"" + input_event->m_name + "\" of module \"" + em->m_name + "\" called succefully from \"" +
                    origin + "\" !");
          } else {
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Trying to call \"" + input_event->m_name + "\" of module \"" + em->m_name + "\" from \"" + origin +
                    "\" but it not exist !");
          }

          event_hit = true;
        }
      }
    }
  }

  // Iterate through each plguins in the Vortex context
  for (auto ep : ctx.IO.ep) {
    // Check if the EventManager corresponds to the specified module
    if (ep->m_name == module_name) {
      // Iterate through each input event in the EventManager
      for (auto input_event : ep->m_input_events) {
        // Check if the input event name matches the provided event name
        if (input_event->m_name == event_name) {
          // Check if the event function pointer is valid
          if (input_event->m_function) {
            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Calling module input event \"" + input_event->m_name + "\" of module \"" + ep->m_name + "\" from \"" +
                    origin + "\"");

            // Call the corresponding event function with the provided arguments
            input_event->m_function(args, ret);

            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Input event \"" + input_event->m_name + "\" of module \"" + ep->m_name + "\" called succefully from \"" +
                    origin + "\" !");
          } else {
            input_event->trigger_happening(
                origin + ":call_input_event",
                HappeningState::INFO,
                "Trying to call \"" + input_event->m_name + "\" of module \"" + ep->m_name + "\" from \"" + origin +
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