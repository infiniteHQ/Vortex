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

#include "dummy_function.h"
#include "event.h"
#include "function.h"
#include "render_instance.h"

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

// Forward declarations
struct ItemHandlerInterface;
struct ItemIdentifierInterface;
struct ItemCreatorInterface;

VORTEX_API struct ModuleInterfaceDep {
  std::string type; // em, plugin, etc..
  std::string name;
  std::vector<std::string> supported_versions;
  bool satisfied;
};

VORTEX_API class ModuleInterface {
public:
  virtual ~ModuleInterface() {}

  // Main functions
  virtual void execute() {};
  virtual void destroy() {};
  virtual void render() {};

  VORTEX_API void Start();
  VORTEX_API void Stop();

  // Hooks
  VORTEX_API void OnInputEvent();
  VORTEX_API void OnOutputEvent();

  // Misc
  VORTEX_API void AddLogo(const uint8_t *hexa, size_t size);
  VORTEX_API void AddLogo(const std::string &relative_path);
  VORTEX_API void ResetModule();

  // Functions of the modules (gives the Vortex abstraction/features)
  VORTEX_API void AddFunction(std::function<void()> foo,
                              const std::string &name);
  VORTEX_API void AddFunction(std::function<void(ArgumentValues &)> foo,
                              const std::string &name);
  VORTEX_API void AddFunction(std::function<void(ReturnValues &)> foo,
                              const std::string &name);
  VORTEX_API void
  AddFunction(std::function<void(ArgumentValues &, ReturnValues &)> foo,
              const std::string &name);
  VORTEX_API void ExecuteFunction(const std::string &name);
  VORTEX_API void ExecuteFunction(const std::string &name,
                                  ArgumentValues &args);
  VORTEX_API void ExecuteFunction(const std::string &name, ReturnValues &ret);
  VORTEX_API void ExecuteFunction(const std::string &name, ArgumentValues &args,
                                  ReturnValues &ret);

  VORTEX_API std::string GetPath();
  VORTEX_API std::string GetBinaryPath();

  // Output Events
  // A output event is triggered via VortexMaker::ExecuteOutputEvent() by the
  // current component to all concerned extern components with the output event
  // added.
  VORTEX_API void AddOutputEvent(std::function<void()> foo,
                                 const std::string &name);
  VORTEX_API void AddOutputEvent(std::function<void(ArgumentValues &)> foo,
                                 const std::string &name);
  VORTEX_API void AddOutputEvent(std::function<void(ReturnValues &)> foo,
                                 const std::string &name);
  VORTEX_API void
  AddOutputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo,
                 const std::string &name);
  VORTEX_API void AddOutputEvent(const ModuleOutputEvent &event);

  // Input Events
  // A input event is triggered via VortexMaker::ExecuteInputEvent() by a extern
  // component to the current component with the input component added.
  VORTEX_API void AddInputEvent(std::function<void()> foo,
                                const std::string &name);
  VORTEX_API void AddInputEvent(std::function<void(ArgumentValues &)> foo,
                                const std::string &name);
  VORTEX_API void AddInputEvent(std::function<void(ReturnValues &)> foo,
                                const std::string &name);
  VORTEX_API void
  AddInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo,
                const std::string &name);
  VORTEX_API void AddInputEvent(const ModuleInputEvent &event);

  // GUI stuffs
  VORTEX_API void RefreshMainWindow();
  VORTEX_API void SetMainWindow(const std::shared_ptr<Cherry::AppWindow> &win);
  VORTEX_API void AddWindow(const std::shared_ptr<Cherry::AppWindow> &win);

  // Content Browser item handler
  VORTEX_API void
  AddContentBrowserItemHandler(const ItemHandlerInterface &handler);
  VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>>
  GetContentBrowserItemHandler();

  // Content Browser item identifier
  VORTEX_API void
  AddContentBrowserItemIdentifier(const ItemIdentifierInterface &item);
  VORTEX_API std::vector<std::shared_ptr<ItemIdentifierInterface>> &
  GetContentBrowserItemIdentifiers();

  // Content Browser item creators
  VORTEX_API void
  AddContentBrowserItemCreator(const ItemCreatorInterface &item);
  VORTEX_API std::vector<std::shared_ptr<ItemCreatorInterface>> &
  GetContentBrowserItemCreators();

  // Logs & Metrics
  VORTEX_API void LogInfo(const std::string &message);
  VORTEX_API void LogWarning(const std::string &message);
  VORTEX_API void LogError(const std::string &message);
  VORTEX_API void LogFatal(const std::string &message);

  // TODO : AddParameterToFunction
  // TODO : AddParameterToInputEvent
  // TODO : AddParameterToOutputEvent
  // TODO : AddReturnToFunction
  // TODO : AddReturnToInputEvent
  // TODO : AddReturnToOutputEvent
  // TODO : AddCommonUtility

  // void AddFunction(void (*item)(), const std::string& name, Parameters
  // params);
  VORTEX_API std::shared_ptr<ModuleInterface> GetInterface();

  VORTEX_API void ExecuteOutputEvent(const std::string &name,
                                     ArgumentValues &args, ReturnValues &ret);
  VORTEX_API void ExecuteInputEvent(const std::string &name,
                                    ArgumentValues &args, ReturnValues &ret);

  // Trigger all output events for every modules/plugins
  VORTEX_API void DeployOutputEvent(const std::string &name,
                                    std::shared_ptr<hArgs> args);
  VORTEX_API void ExecOutputEvent(const std::string &name,
                                  std::shared_ptr<hArgs> args);

  template <typename T>
  VORTEX_API void AddModuleItemParam(const void *item,
                                     std::pair<std::string, T> parameter);

  VORTEX_API void
  AddModuleRenderInstance(const std::shared_ptr<ModuleRenderInstance> &event);
  VORTEX_API void AddModuleFunction(const ModuleFunction &event);
  std::vector<std::shared_ptr<ModuleRenderInstance>> GetModuleRenderInstances();

  VORTEX_API void CallOutputEvent(const std::string &event_name,
                                  ArgumentValues &args, ReturnValues &ret);
  VORTEX_API void CallInputEvent(const std::string &module_name,
                                 const std::string &event_name,
                                 ArgumentValues &args, ReturnValues &ret);

  VORTEX_API static std::shared_ptr<ModuleInterface>
  GetEditorModuleByName(const std::string &name);

  VORTEX_API void CheckDependencies();
  VORTEX_API void CheckVersion();

  std::shared_ptr<hArgs> m_args;
  std::string m_datapath;

public:
  std::string m_type;
  std::string m_proper_name;
  std::string m_name;
  std::string m_version;
  std::string m_path;
  std::string m_binary_path;
  std::string m_author;
  std::string m_group;
  std::string m_picture;
  std::string m_logo_path;
  std::string m_description;
  bool m_auto_exec;
  std::vector<std::string> m_contributors;
  std::vector<std::shared_ptr<ModuleInterfaceDep>> m_dependencies;
  std::vector<std::string> m_supported_versions;
  const uint8_t *m_logo;
  size_t m_logo_size;

  std::string m_state = "unknow";
  std::shared_ptr<Cherry::AppWindow> m_main_window;

  std::vector<std::shared_ptr<ModuleFunction>> m_functions;
  std::vector<std::shared_ptr<ModuleOutputEvent>> m_output_events;
  std::vector<std::shared_ptr<ModuleInputEvent>> m_input_events;
  std::vector<std::shared_ptr<ItemHandlerInterface>> m_item_handlers;
  std::vector<std::shared_ptr<ItemIdentifierInterface>> m_item_identifiers;
  std::vector<std::shared_ptr<ItemCreatorInterface>> m_item_creators;

private:
  std::vector<std::shared_ptr<ModuleDummyFunction>> m_dummy_functions;
  std::vector<std::shared_ptr<ModuleRenderInstance>> m_render_instances;
};

#endif // MODULE_INTERFACE_H