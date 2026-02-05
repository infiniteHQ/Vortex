#include <vortex.h>
#include <vortex_internals.h>

#ifndef PLUGIN_EVENT_H
#define PLUGIN_EVENT_H

// Forward declarations
enum class HappeningState;
enum class DevFlag;
enum class HandlerItemType;

// TODO : Replace all args by json values (input and output)

struct PluginInputEventHappening {
  std::string m_trigger_name;
  HappeningState m_state;
  std::string m_log;
  std::string m_timestamp;
};

struct PluginOutputEventHappening {
  std::string m_trigger_name;
  HappeningState m_state;
  std::string m_log;
  std::string m_timestamp;
};

/**
 * @brief Executed by other with custom parameters
 */
class PluginInputEvent {
public:
  PluginInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo,
                   const std::string &name);
  PluginInputEvent(std::function<void(ArgumentValues &)> foo,
                   const std::string &name);
  PluginInputEvent(std::function<void(ReturnValues &)> foo,
                   const std::string &name);
  PluginInputEvent(std::function<void()> foo, const std::string &name);

  std::function<void(ArgumentValues &, ReturnValues &)> m_function;
  void trigger_happening(const std::string &trigger_name, HappeningState state,
                         const std::string &log);

  std::string m_name;
  std::string m_description;
  std::vector<std::tuple<std::string, std::string, std::string>> m_params;
  bool m_can_callback;
  DevFlag m_devflag;
  std::vector<std::shared_ptr<PluginInputEventHappening>> m_happenings;
};

/**
 * @brief Emmiting this even with custom parameters.
 */
class PluginOutputEvent {
public:
  PluginOutputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo,
                    const std::string &name);
  PluginOutputEvent(std::function<void(ArgumentValues &)> foo,
                    const std::string &name);
  PluginOutputEvent(std::function<void(ReturnValues &)> foo,
                    const std::string &name);
  PluginOutputEvent(std::function<void()> foo, const std::string &name);

  std::function<void(ArgumentValues &, ReturnValues &)> m_function;

  void trigger_happening(const std::string &trigger_name, HappeningState state,
                         const std::string &log);

  virtual void execute() {};

  std::string m_name;
  std::vector<std::shared_ptr<PluginOutputEventHappening>> m_happenings;
};

#endif