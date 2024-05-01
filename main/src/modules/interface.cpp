#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"
#include "../../include/modules/interface.h"

/**
 * @brief Adds a ModuleFunction to the ModuleInterface.
 *
 * This function creates a shared_ptr to the provided ModuleFunction and adds it to the ModuleInterface's list of functions.
 *
 * @param function The ModuleFunction to add.
 */
void ModuleInterface::AddModuleFunction(const ModuleFunction &function)
{
    // Create a shared_ptr to the ModuleFunction
    std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(function);

    // Add the shared_ptr to the list of functions
    this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a ModuleOutputEvent to the ModuleInterface.
 *
 * This function creates a shared_ptr to the provided ModuleOutputEvent and adds it to the ModuleInterface's list of output events.
 *
 * @param event The ModuleOutputEvent to add.
 */
void ModuleInterface::AddModuleOutputEvent(const ModuleOutputEvent &event)
{
    // Create a shared_ptr to the ModuleOutputEvent
    std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(event);

    // Add the shared_ptr to the list of output events
    this->m_output_events.push_back(p_event);
}

void ModuleInterface::AddModuleInputEvent(const ModuleInputEvent &event)
{
}

/**
 * @brief Adds a ModuleRenderInstance to the ModuleInterface.
 *
 * This function adds the provided shared_ptr to a ModuleRenderInstance to the ModuleInterface's list of render instances.
 *
 * @param renderInstance The shared_ptr to the ModuleRenderInstance to add.
 */
void ModuleInterface::AddModuleRenderInstance(const std::shared_ptr<ModuleRenderInstance> &renderInstance)
{
    // Add the shared_ptr to the list of render instances
    this->m_render_instances.push_back(renderInstance);
}

/**
 * @brief Adds an argument to the ModuleInterface's argument container.
 *
 * This function adds a key-value pair to the ModuleInterface's argument container.
 * If the container does not exist, it creates a new one and adds the argument.
 *
 * @tparam T The type of the argument value.
 * @param key The key for the argument.
 * @param value The value of the argument.
 */
template <typename T>
void ModuleInterface::AddArg(const std::string &key, T value)
{
    if (this->m_args == nullptr)
    {
        // Create a new argument container if it doesn't exist
        std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
        this->m_args = args;
    }

    // Add the argument to the container
    this->m_args->add<T>(key.c_str(), value);
}

/**
 * @brief Sets the logo for the ModuleInterface.
 *
 * This function sets the logo data and size for the ModuleInterface.
 *
 * @param hexa Pointer to the logo data as an array of uint8_t.
 * @param size Size of the logo data in bytes.
 */
void ModuleInterface::AddLogo(const uint8_t *hexa, size_t size)
{
    // Set the logo data and size
    this->m_logo = hexa;
    this->m_logo_size = size;
}

void ModuleInterface::AddLogo(const std::string &relative_path)
{
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
void ModuleInterface::AddFunction(void (*item)(), const std::string &name)
{
    // Create a shared_ptr to the ModuleFunction
    std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(item, name);

    // Add the shared_ptr to the list of functions
    this->m_functions.push_back(p_function);
}

/**
 * @brief Adds an input event to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleInputEvent and adds it to the ModuleInterface's list of input events.
 *
 * @param item Pointer to the input event function.
 * @param name Name of the input event.
 */
void ModuleInterface::AddInputEvent(void (*item)(const std::shared_ptr<hArgs> &args), const std::string &name)
{
    // Create a shared_ptr to the ModuleInputEvent
    std::shared_ptr<ModuleInputEvent> p_event = std::make_shared<ModuleInputEvent>(item, name);

    // Add the shared_ptr to the list of input events
    this->m_input_events.push_back(p_event);
}

/**
 * @brief Adds an output event to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleOutputEvent and adds it to the ModuleInterface's list of output events.
 *
 * @param item Pointer to the output event function.
 * @param name Name of the output event.
 */
void ModuleInterface::AddOutputEvent(void (*item)(const std::shared_ptr<hArgs> &args), const std::string &name)
{
    // Create a shared_ptr to the ModuleOutputEvent
    std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(item, name);

    // Add the shared_ptr to the list of output events
    this->m_output_events.push_back(p_event);
}

/**
 * @brief Gets a shared_ptr to a copy of the ModuleInterface.
 *
 * This function creates a shared_ptr to a new ModuleInterface object, copying the contents
 * of the current ModuleInterface instance.
 *
 * @return A shared_ptr to a copy of the ModuleInterface.
 */
std::shared_ptr<ModuleInterface> ModuleInterface::GetInterface()
{
    // Create a shared_ptr to a new ModuleInterface object, copying the current instance
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
std::shared_ptr<ModuleInterface> ModuleInterface::GetEditorModuleByName(const std::string &name)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();

    for (auto em : ctx->IO.em)
    {
        if (name == em->m_name)
        {
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
void ModuleInterface::ExecFunction(const std::string &name)
{
    for (auto foo : this->m_functions)
    {
        if (foo->m_name == name)
        {
            foo->m_foo();
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
 * @param name The name of the input event to execute.
 * @param args The arguments to pass to the input event function.
 */
void ModuleInterface::ExecInputEvent(const std::string &name, std::shared_ptr<hArgs> args)
{
    for (auto event : this->m_input_events)
    {
        if (event->m_name == name)
        {
            event->m_foo(args);
            return; // Exit after executing the input event
        }
    }
}

/**
 * @brief Executes an output event by name.
 *
 * This function searches for a ModuleOutputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 * @param name The name of the output event to execute.
 * @param args The arguments to pass to the output event function.
 */
void ModuleInterface::ExecOutputEvent(const std::string &name, std::shared_ptr<hArgs> args)
{
    for (auto event : this->m_output_events)
    {
        if (event->m_name == name)
        {
            event->m_foo(args);
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
void ModuleInterface::LogInfo(const std::string &message)
{
    VortexMaker::LogInfo(this->m_name, message);
}

/**
 * @brief Logs a warning message.
 *
 * Logs a warning message associated with this module interface.
 *
 * @param message The warning message to log.
 */
void ModuleInterface::LogWarning(const std::string &message)
{
    VortexMaker::LogWarn(this->m_name, message);
}

/**
 * @brief Logs an error message.
 *
 * Logs an error message associated with this module interface.
 *
 * @param message The error message to log.
 */
void ModuleInterface::LogError(const std::string &message)
{
    VortexMaker::LogError(this->m_name, message);
}

/**
 * @brief Logs a fatal error message.
 *
 * Logs a fatal error message associated with this module interface.
 *
 * @param message The fatal error message to log.
 */
void ModuleInterface::LogFatal(const std::string &message)
{
    VortexMaker::LogFatal(this->m_name, message);
}

/**
 * @brief Checks the dependencies of the module interface.
 *
 * This function checks if the dependencies specified for the module interface
 * are satisfied based on the current context.
 */
void ModuleInterface::CheckDependencies()
{
    // Get the current Vortex context
    VxContext *ctx = VortexMaker::GetCurrentContext();

    // Iterate through each dependency
    for (auto dependency : this->m_dependencies)
    {
        // By default, dependency is not satisfied
        dependency->satisfied = false;

        // Check dependency type
        if (dependency->type == "em")
        {
            // Dependency is an entity module
            for (auto ctx_em : ctx->IO.em)
            {
                // Check if context entity module matches the dependency name
                if (ctx_em->m_name == dependency->name)
                {
                    // Check if the context entity module version is among the supported versions
                    for (auto version : dependency->supported_versions)
                    {
                        if (ctx_em->m_version == version)
                        {
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
 * If any dependency is not satisfied, logs a fatal error, lists the failed dependencies,
 * and sets the state to "failed".
 * If all dependencies are satisfied, executes the main function and sets the state to "running".
 */
void ModuleInterface::Start()
{
    // Get the current Vortex context
    VxContext *ctx = VortexMaker::GetCurrentContext();

    // Update dependencies
    this->CheckDependencies();

    // Get current vortex version
    std::string version = VORTEX_VERSION;

    // Get major, a medium idendifiers of version
    std::size_t last_point = version.find('.', version.find('.') + 1);
    std::string major = version.substr(0, last_point);

    if (!major.empty() && major.back() == '.')
    {
        major.pop_back();
    }

    // Check if all dependencies are satisfied
    bool allDependenciesSatisfied = true;
    bool isVersionCompatible = false;

    // Vector of unsatisfied dependencies
    std::vector<std::pair<std::string, std::string>> unsatisfiedDependencies;

    for (auto dependency : this->m_dependencies)
    {
        if (!dependency->satisfied)
        {
            allDependenciesSatisfied = false;
            std::string versions;
            for (auto version : dependency->supported_versions)
            {
                versions += "[" + version + "]";
            }
            unsatisfiedDependencies.push_back({dependency->name, versions});
        }
    }

    for (auto version : this->m_supported_versions)
    {
        if (version == major)
        {
            isVersionCompatible = true;
        }
    }

    // Vector of needed dependencies (to run)
    std::vector<std::pair<std::string, std::string>> needToRunDependencies;
    for (auto dep : this->m_dependencies)
    {
        for (auto em : ctx->IO.em)
        {
            if (em->m_name == dep->name)
            {
                std::string versions;
                for (auto version : dep->supported_versions)
                {
                    versions += "[" + version + "]";
                }

                if (em->m_state != "running")
                {
                    needToRunDependencies.push_back({dep->name, versions});
                }
            }
        }
    }

    // If any dependency is not satisfied, log and set state to "failed"
    if (!allDependenciesSatisfied)
    {
        this->LogError("Dependencies not satisfied!");
        for (auto unsatisfiedDep : unsatisfiedDependencies)
        {
            this->LogError("Failed to retrieve: " + unsatisfiedDep.first + " with version(s) " + unsatisfiedDep.second);
        }
        this->m_state = "failed";
        return;
    }

    if (!isVersionCompatible)
    {
        this->LogError("The Vortex version (" + version + ", try to find \"" + major + "\") is incompatible with \"" + this->m_name + "\" supported version(s).");

        this->m_state = "failed";
        return;
    }

    if (!needToRunDependencies.empty())
    {
        this->LogError("Dependencies of this modules are not running !");

        for (auto notRunningDeps : needToRunDependencies)
        {
            this->LogError("Please run : " + notRunningDeps.first + " with version(s) " + notRunningDeps.second);
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
 * Calls the destroy function to clean up resources and sets the state to "stopped".
 */
void ModuleInterface::Stop()
{
    // Get the current Vortex context
    VxContext *ctx = VortexMaker::GetCurrentContext();

    bool authorized = true;
    std::vector<std::pair<std::string, std::string>> deps;

    for (auto em : ctx->IO.em)
    {
        if (em->m_state == "running")
        {
            for (auto dependency : em->m_dependencies)
            {
                if (dependency->name == this->m_name)
                {
                    for (auto version : dependency->supported_versions)
                    {
                        if (this->m_version == version)
                        {
                            authorized = false;

                            std::string versions;
                            for (auto version : dependency->supported_versions)
                            {
                                versions += "[" + version + "]";
                            }
                            deps.push_back({dependency->name, versions});
                        }
                    }
                }
            }
        }
    }

    if (!authorized)
    {
        this->LogError("Cannot stop \"" + this->m_name + "\" ! ");
        for (auto dep : deps)
        {
            this->LogError("This module is used by \"" + dep.first + "\" with needed versions : " + dep.second + " ! ");
        }
    }
    else
    {
        this->destroy();
        this->m_state = "stopped";
    }
}

void ModuleInterface::CheckVersion()
{
}

std::vector<std::shared_ptr<ModuleRenderInstance>> ModuleInterface::GetModuleRenderInstances()
{
    return this->m_render_instances;
};