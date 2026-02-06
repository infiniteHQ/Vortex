#include "../../include/plugins/interface.h"
#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Adds a PluginFunction to the PluginInterface.
 *
 * This function creates a shared_ptr to the provided PluginFunction and adds it
 * to the PluginInterface's list of functions.
 *
 * @param function The PluginFunction to add.
 */
VORTEX_API void
PluginInterface::AddPluginFunction(const PluginFunction &function) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function =
      std::make_shared<PluginFunction>(function);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a PluginOutputEvent to the PluginInterface.
 *
 * This function creates a shared_ptr to the provided PluginOutputEvent and adds
 * it to the PluginInterface's list of output events.
 *
 * @param event The PluginOutputEvent to add.
 */
VORTEX_API void
PluginInterface::AddOutputEvent(const PluginOutputEvent &event) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event =
      std::make_shared<PluginOutputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void PluginInterface::AddInputEvent(const PluginInputEvent &event) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event =
      std::make_shared<PluginInputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void PluginInterface::AddContentBrowserItemHandler(
    const ItemHandlerInterface &handler) {
  m_item_handlers.push_back(std::make_shared<ItemHandlerInterface>(handler));
}

VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>>
PluginInterface::GetContentBrowserItemHandler() {
  return m_item_handlers;
}

VORTEX_API std::string PluginInterface::GetPath() { return m_path; }
VORTEX_API std::string PluginInterface::GetMainScriptPath() {
  return m_mainscript_path;
}

VORTEX_API void PluginInterface::RefreshMainWindow() {
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

VORTEX_API void
PluginInterface::SetMainWindow(const std::shared_ptr<Cherry::AppWindow> &win) {
  m_main_window = win;

  if (&Cherry::Application::Get()) {
    RefreshMainWindow();
  }
}

/**
 * @brief Adds a PluginRenderInstance to the PluginInterface.
 *
 * This function adds the provided shared_ptr to a PluginRenderInstance to the
 * PluginInterface's list of render instances.
 *
 * @param renderInstance The shared_ptr to the PluginRenderInstance to add.
 */
VORTEX_API void PluginInterface::AddPluginRenderInstance(
    const std::shared_ptr<PluginRenderInstance> &renderInstance) {
  // Add the shared_ptr to the list of render instances
  this->m_render_instances.push_back(renderInstance);
}

/**
 * @brief Sets the logo for the PluginInterface.
 *
 * This function sets the logo data and size for the PluginInterface.
 *
 * @param hexa Pointer to the logo data as an array of uint8_t.
 * @param size Size of the logo data in bytes.
 */
VORTEX_API void PluginInterface::AddLogo(const uint8_t *hexa, size_t size) {
  // Set the logo data and size
  this->m_logo = hexa;
  this->m_logo_size = size;
}

VORTEX_API void PluginInterface::AddLogo(const std::string &relative_path) {}

VORTEX_API void PluginInterface::AddOutputEvent(std::function<void()> foo,
                                                const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event =
      std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void
PluginInterface::AddOutputEvent(std::function<void(ArgumentValues &)> foo,
                                const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event =
      std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void
PluginInterface::AddOutputEvent(std::function<void(ReturnValues &)> foo,
                                const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event =
      std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void PluginInterface::AddOutputEvent(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event =
      std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void PluginInterface::AddInputEvent(std::function<void()> foo,
                                               const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event =
      std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void
PluginInterface::AddInputEvent(std::function<void(ArgumentValues &)> foo,
                               const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event =
      std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void
PluginInterface::AddInputEvent(std::function<void(ReturnValues &)> foo,
                               const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event =
      std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void PluginInterface::AddInputEvent(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event =
      std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

/**
 * @brief Adds a function to the PluginInterface.
 *
 * This function creates a shared_ptr to a PluginFunction and adds it to the
 * PluginInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void PluginInterface::AddFunction(std::function<void()> foo,
                                             const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function =
      std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a function to the PluginInterface.
 *
 * This function creates a shared_ptr to a PluginFunction and adds it to the
 * PluginInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void
PluginInterface::AddFunction(std::function<void(ArgumentValues &)> foo,
                             const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function =
      std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a function to the PluginInterface.
 *
 * This function creates a shared_ptr to a PluginFunction and adds it to the
 * PluginInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void
PluginInterface::AddFunction(std::function<void(ReturnValues &)> foo,
                             const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function =
      std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a function to the PluginInterface.
 *
 * This function creates a shared_ptr to a PluginFunction and adds it to the
 * PluginInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void PluginInterface::AddFunction(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function =
      std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Gets a shared_ptr to a copy of the PluginInterface.
 *
 * This function creates a shared_ptr to a new PluginInterface object, copying
 * the contents of the current PluginInterface instance.
 *
 * @return A shared_ptr to a copy of the PluginInterface.
 */
VORTEX_API std::shared_ptr<PluginInterface> PluginInterface::GetInterface() {
  // Create a shared_ptr to a new PluginInterface object, copying the current
  // instance
  return std::make_shared<PluginInterface>(*this);
}

/**
 * @brief Gets an editor plugin by name.
 *
 * This function searches for an editor plugin with the specified name
 * and returns a shared_ptr to it if found.
 *
 * @param name The name of the editor plugin to search for.
 * @return A shared_ptr to the editor plugin if found, otherwise nullptr.
 */
VORTEX_API std::shared_ptr<PluginInterface>
PluginInterface::GetEditorPluginByName(const std::string &name) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  for (auto ep : ctx->IO.ep) {
    if (name == ep->m_name) {
      // Return a shared_ptr to the editor plugin if found
      return ep;
    }
  }

  // Return nullptr if editor plugin with the specified name is not found
  return nullptr;
}

/**
 * @brief Executes a function by name.
 *
 * This function searches for a PluginFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void PluginInterface::ExecuteFunction(const std::string &name) {
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
 * This function searches for a PluginFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void PluginInterface::ExecuteFunction(const std::string &name,
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
 * This function searches for a PluginFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void PluginInterface::ExecuteFunction(const std::string &name,
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
 * This function searches for a PluginFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void PluginInterface::ExecuteFunction(const std::string &name,
                                                 ArgumentValues &args,
                                                 ReturnValues &ret) {
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
 * This function searches for a PluginInputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 */
VORTEX_API void PluginInterface::ExecuteInputEvent(const std::string &name,
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
 * This function searches for a PluginInputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 */
VORTEX_API void PluginInterface::ExecuteOutputEvent(const std::string &name,
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
 * Logs an informational message associated with this plugin interface.
 *
 * @param message The message to log.
 */
VORTEX_API void PluginInterface::LogInfo(const std::string &message) {
  VortexMaker::LogInfo(this->m_name, message);
}

/**
 * @brief Logs a warning message.
 *
 * Logs a warning message associated with this plugin interface.
 *
 * @param message The warning message to log.
 */
VORTEX_API void PluginInterface::LogWarning(const std::string &message) {
  VortexMaker::LogWarn(this->m_name, message);
}

/**
 * @brief Logs an error message.
 *
 * Logs an error message associated with this plugin interface.
 *
 * @param message The error message to log.
 */
VORTEX_API void PluginInterface::LogError(const std::string &message) {
  VortexMaker::LogError(this->m_name, message);
}

/**
 * @brief Logs a fatal error message.
 *
 * Logs a fatal error message associated with this plugin interface.
 *
 * @param message The fatal error message to log.
 */
VORTEX_API void PluginInterface::LogFatal(const std::string &message) {
  VortexMaker::LogFatal(this->m_name, message);
}

/**
 * @brief Checks the dependencies of the plugin interface.
 *
 * This function checks if the dependencies specified for the plugin interface
 * are satisfied based on the current context.
 */
VORTEX_API void PluginInterface::CheckDependencies() {
  // Get the current Vortex context
  VxContext *ctx = VortexMaker::GetCurrentContext();

  // Iterate through each dependency
  for (auto dependency : this->m_dependencies) {
    // By default, dependency is not satisfied
    dependency->satisfied = false;

    // Check dependency type
    if (dependency->type == "em") {
      // Dependency is an entity plugin
      for (auto ctx_em : ctx->IO.em) {
        // Check if context entity plugin matches the dependency name
        if (ctx_em->m_name == dependency->name) {
          // Check if the context entity plugin version is among the supported
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
 * @brief Starts the plugin interface.
 *
 * Checks dependencies and executes the plugin's main function.
 * If any dependency is not satisfied, logs a fatal error, lists the failed
 * dependencies, and sets the state to "failed". If all dependencies are
 * satisfied, executes the main function and sets the state to "running".
 */
VORTEX_API void PluginInterface::Start() {
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
        "This plugin isn't compatible with this version of Vortex (\"" + major +
        "\") ! Is incompatible with \"" + this->m_name +
        "\" supported version(s).");

    this->m_state = "failed";
    return;
  }

  if (!needToRunDependencies.empty()) {
    this->LogError("Dependencies of this plugins are not running !");

    for (auto notRunningDeps : needToRunDependencies) {
      this->LogError("Please run : " + notRunningDeps.first +
                     " with version(s) " + notRunningDeps.second);
    }
    this->m_state = "failed";
    return;
  }

  // Execute the plugin's main script here
  const std::string script = this->m_mainscript_path;
  VortexMaker::Script::RenderLuaFreshScript(script);

  this->m_state = "running";
}

/**
 * @brief Stops the plugin interface.
 *
 * Calls the destroy function to clean up resources and sets the state to
 * "stopped".
 */
VORTEX_API void PluginInterface::Stop() {
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
      this->LogError("This plugin is used by \"" + dep.first +
                     "\" with needed versions : " + dep.second + " ! ");
    }
  } else {
    this->destroy();
    this->m_state = "stopped";
  }
}

VORTEX_API void PluginInterface::ResetPlugin() { m_item_handlers.clear(); }

VORTEX_API void PluginInterface::CallOutputEvent(const std::string &event_name,
                                                 ArgumentValues &args,
                                                 ReturnValues &ret) {
  VortexMaker::CallOutputEvent(event_name, args, ret, this->m_name);
}

VORTEX_API void PluginInterface::CallInputEvent(const std::string &plugin_name,
                                                const std::string &event_name,
                                                ArgumentValues &args,
                                                ReturnValues &ret) {
  VortexMaker::CallInputEvent(plugin_name, event_name, args, ret, this->m_name);
}

VORTEX_API void PluginInterface::CheckVersion() {}

std::vector<std::shared_ptr<PluginRenderInstance>>
PluginInterface::GetPluginRenderInstances() {
  return this->m_render_instances;
};

VORTEX_API void PluginInterface::AddContentBrowserItemIdentifier(
    const ItemIdentifierInterface &item) {
  m_item_identifiers.push_back(std::make_shared<ItemIdentifierInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemIdentifierInterface>> &
PluginInterface::GetContentBrowserItemIdentifiers() {
  return m_item_identifiers;
}

VORTEX_API void PluginInterface::AddContentBrowserItemCreator(
    const ItemCreatorInterface &item) {
  m_item_creators.push_back(std::make_shared<ItemCreatorInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemCreatorInterface>> &
PluginInterface::GetContentBrowserItemCreators() {
  return m_item_creators;
}
