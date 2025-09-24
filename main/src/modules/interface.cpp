#include "../../include/modules/interface.h"
#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Adds a ModuleFunction to the ModuleInterface.
 *
 * This function creates a shared_ptr to the provided ModuleFunction and adds it
 * to the ModuleInterface's list of functions.
 *
 * @param function The ModuleFunction to add.
 */
VORTEX_API void ModuleInterface::AddModuleFunction(const ModuleFunction &function) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function =
      std::make_shared<ModuleFunction>(function);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a ModuleOutputEvent to the ModuleInterface.
 *
 * This function creates a shared_ptr to the provided ModuleOutputEvent and adds
 * it to the ModuleInterface's list of output events.
 *
 * @param event The ModuleOutputEvent to add.
 */
VORTEX_API void ModuleInterface::AddOutputEvent(const ModuleOutputEvent &event) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event =
      std::make_shared<ModuleOutputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddInputEvent(const ModuleInputEvent &event) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event =
      std::make_shared<ModuleInputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddContentBrowserItemHandler(
    const ItemHandlerInterface &handler) {
  m_item_handlers.push_back(std::make_shared<ItemHandlerInterface>(handler));
}

VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>>
ModuleInterface::GetContentBrowserItemHandler() {
  return m_item_handlers;
}

VORTEX_API std::string ModuleInterface::GetPath() { return m_path; }

VORTEX_API void ModuleInterface::RefreshMainWindow() {
  if (!&Cherry::Application::Get()) {
    return;
  }

  // Remove potential old main window
  for (auto &window : Cherry::Application::Get().m_AppWindows) {
    if (m_main_window && window) {

      if (m_main_window->m_IdName == window->m_IdName) {
        Cherry::DeleteAppWindow(window);
      }
    }
  }

  // Add/Set the new one
  if (m_main_window) {
    Cherry::AddAppWindow(m_main_window);
  }
}

VORTEX_API void ModuleInterface::SetMainWindow(
    const std::shared_ptr<Cherry::AppWindow> &win) {
  m_main_window = win;

  if (&Cherry::Application::Get()) {
    RefreshMainWindow();
  }
}

/**
 * @brief Adds a ModuleRenderInstance to the ModuleInterface.
 *
 * This function adds the provided shared_ptr to a ModuleRenderInstance to the
 * ModuleInterface's list of render instances.
 *
 * @param renderInstance The shared_ptr to the ModuleRenderInstance to add.
 */
VORTEX_API void ModuleInterface::AddModuleRenderInstance(
    const std::shared_ptr<ModuleRenderInstance> &renderInstance) {
  // Add the shared_ptr to the list of render instances
  this->m_render_instances.push_back(renderInstance);
}

/**
 * @brief Sets the logo for the ModuleInterface.
 *
 * This function sets the logo data and size for the ModuleInterface.
 *
 * @param hexa Pointer to the logo data as an array of uint8_t.
 * @param size Size of the logo data in bytes.
 */
VORTEX_API void ModuleInterface::AddLogo(const uint8_t *hexa, size_t size) {
  // Set the logo data and size
  this->m_logo = hexa;
  this->m_logo_size = size;
}

VORTEX_API void ModuleInterface::AddLogo(const std::string &relative_path) {}

VORTEX_API void ModuleInterface::AddOutputEvent(std::function<void()> foo,
                                     const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event =
      std::make_shared<ModuleOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddOutputEvent(std::function<void(ArgumentValues &)> foo,
                                     const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event =
      std::make_shared<ModuleOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddOutputEvent(std::function<void(ReturnValues &)> foo,
                                     const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event =
      std::make_shared<ModuleOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddOutputEvent(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event =
      std::make_shared<ModuleOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddInputEvent(std::function<void()> foo,
                                    const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event =
      std::make_shared<ModuleInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddInputEvent(std::function<void(ArgumentValues &)> foo,
                                    const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event =
      std::make_shared<ModuleInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddInputEvent(std::function<void(ReturnValues &)> foo,
                                    const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event =
      std::make_shared<ModuleInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void ModuleInterface::AddInputEvent(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event =
      std::make_shared<ModuleInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the
 * ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void ModuleInterface::AddFunction(std::function<void()> foo,
                                  const std::string &name) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function =
      std::make_shared<ModuleFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the
 * ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void ModuleInterface::AddFunction(std::function<void(ArgumentValues &)> foo,
                                  const std::string &name) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function =
      std::make_shared<ModuleFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the
 * ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void ModuleInterface::AddFunction(std::function<void(ReturnValues &)> foo,
                                  const std::string &name) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function =
      std::make_shared<ModuleFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the
 * ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void ModuleInterface::AddFunction(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function =
      std::make_shared<ModuleFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Gets a shared_ptr to a copy of the ModuleInterface.
 *
 * This function creates a shared_ptr to a new ModuleInterface object, copying
 * the contents of the current ModuleInterface instance.
 *
 * @return A shared_ptr to a copy of the ModuleInterface.
 */
VORTEX_API std::shared_ptr<ModuleInterface> ModuleInterface::GetInterface() {
  // Create a shared_ptr to a new ModuleInterface object, copying the current
  // instance
  return std::make_shared<ModuleInterface>(*this);
}

/**
 * @brief Gets an editor module by name.
 *
 * This function searches for an editor module with the specified name
 * and returns a shared_ptr to it if found.
 *
 * @param name The name of the editor module to search for.
 * @return A shared_ptr to the editor module if found, otherwise nullptr.
 */
VORTEX_API std::shared_ptr<ModuleInterface>
ModuleInterface::GetEditorModuleByName(const std::string &name) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  for (auto em : ctx->IO.em) {
    if (name == em->m_name) {
      // Return a shared_ptr to the editor module if found
      return em;
    }
  }

  // Return nullptr if editor module with the specified name is not found
  return nullptr;
}

/**
 * @brief Executes a function by name.
 *
 * This function searches for a ModuleFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void ModuleInterface::ExecuteFunction(const std::string &name) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      ArgumentValues empty_args;
      ReturnValues empty_ret;
      foo->m_function(empty_args, empty_ret);
      return; // Exit after executing the function
    }
  }
}

/**
 * @brief Executes a function by name with arguments.
 *
 * This function searches for a ModuleFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void ModuleInterface::ExecuteFunction(const std::string &name,
                                      ReturnValues &ret) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      ArgumentValues empty_args;
      foo->m_function(empty_args, ret);
      return; // Exit after executing the function
    }
  }
}

/**
 * @brief Executes a function by name with arguments.
 *
 * This function searches for a ModuleFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void ModuleInterface::ExecuteFunction(const std::string &name,
                                      ArgumentValues &args) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      ReturnValues empty_ret;
      foo->m_function(args, empty_ret);
      return; // Exit after executing the function
    }
  }
}

/**
 * @brief Executes a function by name with arguments.
 *
 * This function searches for a ModuleFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void ModuleInterface::ExecuteFunction(const std::string &name,
                                      ArgumentValues &args, ReturnValues &ret) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      foo->m_function(args, ret);
      return; // Exit after executing the function
    }
  }
}

/**
 * @brief Executes an input event by name.
 *
 * This function searches for a ModuleInputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 */
VORTEX_API void ModuleInterface::ExecuteInputEvent(const std::string &name,
                                        ArgumentValues &args,
                                        ReturnValues &ret) {
  for (auto event : this->m_input_events) {
    if (event->m_name == name) {
      event->m_function(args, ret);
      return; // Exit after executing the input event
    }
  }
}
/**
 * @brief Executes an input event by name.
 *
 * This function searches for a ModuleInputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 */
VORTEX_API void ModuleInterface::ExecuteOutputEvent(const std::string &name,
                                         ArgumentValues &args,
                                         ReturnValues &ret) {
  for (auto event : this->m_output_events) {
    if (event->m_name == name) {
      event->m_function(args, ret);
      return; // Exit after executing the output event
    }
  }
}

/**
 * @brief Logs an informational message.
 *
 * Logs an informational message associated with this module interface.
 *
 * @param message The message to log.
 */
VORTEX_API void ModuleInterface::LogInfo(const std::string &message) {
  VortexMaker::LogInfo(this->m_name, message);
}

/**
 * @brief Logs a warning message.
 *
 * Logs a warning message associated with this module interface.
 *
 * @param message The warning message to log.
 */
VORTEX_API void ModuleInterface::LogWarning(const std::string &message) {
  VortexMaker::LogWarn(this->m_name, message);
}

/**
 * @brief Logs an error message.
 *
 * Logs an error message associated with this module interface.
 *
 * @param message The error message to log.
 */
VORTEX_API void ModuleInterface::LogError(const std::string &message) {
  VortexMaker::LogError(this->m_name, message);
}

/**
 * @brief Logs a fatal error message.
 *
 * Logs a fatal error message associated with this module interface.
 *
 * @param message The fatal error message to log.
 */
VORTEX_API void ModuleInterface::LogFatal(const std::string &message) {
  VortexMaker::LogFatal(this->m_name, message);
}

/**
 * @brief Checks the dependencies of the module interface.
 *
 * This function checks if the dependencies specified for the module interface
 * are satisfied based on the current context.
 */
VORTEX_API void ModuleInterface::CheckDependencies() {
  // Get the current Vortex context
  VxContext *ctx = VortexMaker::GetCurrentContext();

  // Iterate through each dependency
  for (auto dependency : this->m_dependencies) {
    // By default, dependency is not satisfied
    dependency->satisfied = false;

    // Check dependency type
    if (dependency->type == "em") {
      // Dependency is an entity module
      for (auto ctx_em : ctx->IO.em) {
        // Check if context entity module matches the dependency name
        if (ctx_em->m_name == dependency->name) {
          // Check if the context entity module version is among the supported
          // versions
          for (auto version : dependency->supported_versions) {
            if (ctx_em->m_version == version) {
              // Dependency is satisfied
              dependency->satisfied = true;
              break; // No need to check further versions
            }
          }
        }
      }
    }
    // Futures deps types... (plugins, etc...)
  }
}

/**
 * @brief Starts the module interface.
 *
 * Checks dependencies and executes the module's main function.
 * If any dependency is not satisfied, logs a fatal error, lists the failed
 * dependencies, and sets the state to "failed". If all dependencies are
 * satisfied, executes the main function and sets the state to "running".
 */
VORTEX_API void ModuleInterface::Start() {
  // Get the current Vortex context
  VxContext *ctx = VortexMaker::GetCurrentContext();

  // Update dependencies
  this->CheckDependencies();

  // Get current vortex version
  std::string version = VORTEX_VERSION;

  std::size_t first_dot = version.find('.');
  std::size_t second_dot = version.find('.', first_dot + 1);

  std::string major = version.substr(0, second_dot);

  if (!major.empty() && major.back() == '.') {
    major.pop_back();
  }

  // Check if all dependencies are satisfied
  bool allDependenciesSatisfied = true;
  bool isVersionCompatible = false;

  // Vector of unsatisfied dependencies
  std::vector<std::pair<std::string, std::string>> unsatisfiedDependencies;

  for (auto dependency : this->m_dependencies) {
    if (!dependency->satisfied) {
      allDependenciesSatisfied = false;
      std::string versions;
      for (auto version : dependency->supported_versions) {
        versions += "[" + version + "]";
      }
      unsatisfiedDependencies.push_back({dependency->name, versions});
    }
  }

  for (auto version : this->m_supported_versions) {
    if (version == major) {
      std::cout << version << major << std::endl;
      isVersionCompatible = true;
    }
  }

  // Vector of needed dependencies (to run)
  std::vector<std::pair<std::string, std::string>> needToRunDependencies;
  for (auto dep : this->m_dependencies) {
    for (auto em : ctx->IO.em) {
      if (em->m_name == dep->name) {
        std::string versions;
        for (auto version : dep->supported_versions) {
          versions += "[" + version + "]";
        }

        if (em->m_state != "running") {
          needToRunDependencies.push_back({dep->name, versions});
        }
      }
    }
  }

  // If any dependency is not satisfied, log and set state to "failed"
  if (!allDependenciesSatisfied) {
    this->LogError("Dependencies not satisfied!");
    for (auto unsatisfiedDep : unsatisfiedDependencies) {
      this->LogError("Failed to retrieve: " + unsatisfiedDep.first +
                     " with version(s) " + unsatisfiedDep.second);
    }
    this->m_state = "failed";
    return;
  }

  if (!isVersionCompatible) {
    this->LogError(
        "This module isn't compatible with this version of Vortex (\"" + major +
        "\") ! Is incompatible with \"" + this->m_name +
        "\" supported version(s).");

    this->m_state = "failed";
    return;
  }

  if (!needToRunDependencies.empty()) {
    this->LogError("Dependencies of this modules are not running !");

    for (auto notRunningDeps : needToRunDependencies) {
      this->LogError("Please run : " + notRunningDeps.first +
                     " with version(s) " + notRunningDeps.second);
    }
    this->m_state = "failed";
    return;
  }

  // Execute the module's main function and set state to "running"
  this->execute();
  this->m_state = "running";
}

/**
 * @brief Stops the module interface.
 *
 * Calls the destroy function to clean up resources and sets the state to
 * "stopped".
 */
VORTEX_API void ModuleInterface::Stop() {
  // Get the current Vortex context
  VxContext *ctx = VortexMaker::GetCurrentContext();

  bool authorized = true;
  std::vector<std::pair<std::string, std::string>> deps;

  for (auto em : ctx->IO.em) {
    if (em->m_state == "running") {
      for (auto dependency : em->m_dependencies) {
        if (dependency->name == this->m_name) {
          for (auto version : dependency->supported_versions) {
            if (this->m_version == version) {
              authorized = false;

              std::string versions;
              for (auto version : dependency->supported_versions) {
                versions += "[" + version + "]";
              }
              deps.push_back({dependency->name, versions});
            }
          }
        }
      }
    }
  }

  if (!authorized) {
    this->LogError("Cannot stop \"" + this->m_name + "\" ! ");
    for (auto dep : deps) {
      this->LogError("This module is used by \"" + dep.first +
                     "\" with needed versions : " + dep.second + " ! ");
    }
  } else {
    this->destroy();
    this->m_state = "stopped";
  }
}

VORTEX_API void ModuleInterface::ResetModule() { m_item_handlers.clear(); }

VORTEX_API void ModuleInterface::CallOutputEvent(const std::string &event_name,
                                      ArgumentValues &args, ReturnValues &ret) {
  VortexMaker::CallOutputEvent(event_name, args, ret, this->m_name);
}

VORTEX_API void ModuleInterface::CallInputEvent(const std::string &module_name,
                                     const std::string &event_name,
                                     ArgumentValues &args, ReturnValues &ret) {
  VortexMaker::CallInputEvent(module_name, event_name, args, ret, this->m_name);
}

VORTEX_API void ModuleInterface::CheckVersion() {}

std::vector<std::shared_ptr<ModuleRenderInstance>>
ModuleInterface::GetModuleRenderInstances() {
  return this->m_render_instances;
};

VORTEX_API void ModuleInterface::AddContentBrowserItemIdentifier(
    const ItemIdentifierInterface &item) {
  m_item_identifiers.push_back(std::make_shared<ItemIdentifierInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemIdentifierInterface>> &
ModuleInterface::GetContentBrowserItemIdentifiers() {
  return m_item_identifiers;
}

VORTEX_API void ModuleInterface::AddContentBrowserItemCreator(
    const ItemCreatorInterface &item) {
  m_item_creators.push_back(std::make_shared<ItemCreatorInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemCreatorInterface>> &
ModuleInterface::GetContentBrowserItemCreators() {
  return m_item_creators;
}
