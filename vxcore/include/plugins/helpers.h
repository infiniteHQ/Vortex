#include <vortex.h>
#include <vortex_internals.h>

#ifndef PLUGIN_HELPERS_H
#define PLUGIN_HELPERS_H

// Forward declarations
enum class HappeningState;
enum class DevFlag;
enum class HandlerItemType;

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

#endif