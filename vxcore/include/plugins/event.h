#include <vortex.h>
#include <vortex_internals.h>

#ifndef PLUGIN_EVENT_H
#define PLUGIN_EVENT_H

#include "helpers.h"

class PluginInputEvent {
 public:
  PluginInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  PluginInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name);
  PluginInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
  PluginInputEvent(std::function<void()> foo, const std::string &name);

  void trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log);

  std::function<void(ArgumentValues &, ReturnValues &)> m_function;
  std::string m_name;
  std::string m_description;
  std::vector<std::tuple<std::string, std::string, std::string>> m_params;
  bool m_can_callback;
  DevFlag m_devflag;
  std::vector<std::shared_ptr<PluginInputEventHappening>> m_happenings;
};

class PluginOutputEvent {
 public:
  PluginOutputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  PluginOutputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name);
  PluginOutputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
  PluginOutputEvent(std::function<void()> foo, const std::string &name);

  void trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log);

  virtual void execute() { };

  std::function<void(ArgumentValues &, ReturnValues &)> m_function;
  std::string m_name;
  std::vector<std::shared_ptr<PluginOutputEventHappening>> m_happenings;
};

#endif