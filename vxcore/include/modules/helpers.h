#include <vortex_internals.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#if defined(__linux__)
#include <cxxabi.h>
#elif defined(_WIN32) || defined(_WIN64)
//
#elif defined(__APPLE__)
//
#else
//
#endif

#ifndef MODULE_HELPERS_H
#define MODULE_HELPERS_H

// Forward declarations
struct ItemHandlerInterface;
struct ItemIdentifierInterface;
struct ItemCreatorInterface;

enum class HappeningState;
enum class DevFlag;
enum class HandlerItemType;

VORTEX_API struct ModuleInterfaceDep {
  std::string type;  // em, plugin, etc..
  std::string name;
  std::vector<std::string> supported_versions;
  bool satisfied;
};

struct ModuleInputEventHappening {
  std::string m_trigger_name;
  HappeningState m_state;
  std::string m_log;
  std::string m_timestamp;
};

struct ModuleOutputEventHappening {
  std::string m_trigger_name;
  HappeningState m_state;
  std::string m_log;
  std::string m_timestamp;
};

#endif  // MODULE_HELPERS_H