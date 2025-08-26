#include "../vortex.h"
#include <vector>
#include <memory>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <stdexcept>

#if defined(__linux__)
#include <cxxabi.h> 
#elif defined(_WIN32) || defined(_WIN64)
//
#elif defined(__APPLE__)
//
#else
//
#endif

#include "function.h"
#include "event.h"
#include "render_instance.h"
#include "dummy_function.h"

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

struct ModuleInterfaceDep
{
    std::string type; // em, plugin, etc..
    std::string name;
    std::vector<std::string> supported_versions;
    bool satisfied;
};

class ModuleInterface
{
    // Module API (declarations needs to be here)
public:
    virtual ~ModuleInterface() {}

    // Main functions
    virtual void execute()  {};
    virtual void destroy()  {};
    virtual void render()   {};

    void Start();
    void Stop();

    // Hooks
    void OnInputEvent();
    void OnOutputEvent();
    
    // Misc
    void AddLogo(const uint8_t* hexa, size_t size);
    void AddLogo(const std::string& relative_path);

    // Functions of the modules (gives the Vortex abstraction/features)
    void AddFunction(std::function<void()> foo, const std::string& name);
    void AddFunction(std::function<void(ArgumentValues&)> foo, const std::string& name);
    void AddFunction(std::function<void(ReturnValues&)> foo, const std::string& name);
    void AddFunction(std::function<void(ArgumentValues&, ReturnValues&)> foo, const std::string& name);
    void ExecuteFunction(const std::string& name);
    void ExecuteFunction(const std::string& name, ArgumentValues& args);
    void ExecuteFunction(const std::string& name, ReturnValues& ret);
    void ExecuteFunction(const std::string& name, ArgumentValues& args, ReturnValues& ret);

    // Output Events
    // A output event is triggered via VortexMaker::ExecuteOutputEvent() by the current component to all concerned extern components with the output event added.
    void AddOutputEvent(std::function<void()> foo, const std::string& name);
    void AddOutputEvent(std::function<void(ArgumentValues&)> foo, const std::string& name);
    void AddOutputEvent(std::function<void(ReturnValues&)> foo, const std::string& name);
    void AddOutputEvent(std::function<void(ArgumentValues&, ReturnValues&)> foo, const std::string& name);
    void AddOutputEvent(const ModuleOutputEvent& event);

    // Input Events
    // A input event is triggered via VortexMaker::ExecuteInputEvent() by a extern component to the current component with the input component added.
    void AddInputEvent(std::function<void()> foo, const std::string& name);
    void AddInputEvent(std::function<void(ArgumentValues&)> foo, const std::string& name);
    void AddInputEvent(std::function<void(ReturnValues&)> foo, const std::string& name);
    void AddInputEvent(std::function<void(ArgumentValues&, ReturnValues&)> foo, const std::string& name);
    void AddInputEvent(const ModuleInputEvent& event);

    // GUI stuffs
    void RefreshMainWindow();
    void SetMainWindow(const std::shared_ptr<Cherry::AppWindow>& win);
    void AddWindow(const std::shared_ptr<Cherry::AppWindow>& win);

    // Content Browser stuffs
    void AddContentBrowserItemHandler();
    void AddContentBrowserItemIdentifier();

    // Logs & Metrics
    void LogInfo(const std::string& message);
    void LogWarning(const std::string& message);
    void LogError(const std::string& message);
    void LogFatal(const std::string& message);

    // TODO : AddParameterToFunction
    // TODO : AddParameterToInputEvent
    // TODO : AddParameterToOutputEvent
    // TODO : AddReturnToFunction
    // TODO : AddReturnToInputEvent
    // TODO : AddReturnToOutputEvent
    // TODO : AddCommonUtility

    //void AddFunction(void (*item)(), const std::string& name, Parameters params);
    std::shared_ptr<ModuleInterface> GetInterface();


    void ExecuteOutputEvent(const std::string& name, ArgumentValues& args, ReturnValues& ret);
    void ExecuteInputEvent(const std::string& name, ArgumentValues& args, ReturnValues& ret); 


    // Trigger all output events for every modules/plugins
    void DeployOutputEvent(const std::string& name, std::shared_ptr<hArgs> args);
    void ExecOutputEvent(const std::string& name, std::shared_ptr<hArgs> args);
    
    template<typename T>
    void AddModuleItemParam(const void *item, std::pair<std::string, T> parameter);
    
    void AddModuleRenderInstance(const std::shared_ptr<ModuleRenderInstance> &event);
    void AddModuleFunction(const ModuleFunction& event);
    std::vector<std::shared_ptr<ModuleRenderInstance>> GetModuleRenderInstances();


    void CallOutputEvent(const std::string &event_name, ArgumentValues& args, ReturnValues& ret);
    void CallInputEvent(const std::string &module_name, const std::string &event_name, ArgumentValues& args, ReturnValues& ret);


    static std::shared_ptr<ModuleInterface> GetEditorModuleByName(const std::string& name);

    void CheckDependencies();
    void CheckVersion();

    std::shared_ptr<hArgs> m_args;
    std::string m_datapath;
public:
    std::string m_type;
    std::string m_proper_name;
    std::string m_name;
    std::string m_version;
    std::string m_path;
    std::string m_author;
    std::string m_group;
    std::string m_picture;
    std::string m_logo_path;
    std::string m_description;
    bool        m_auto_exec;
    std::vector<std::string> m_contributors;
    std::vector<std::shared_ptr<ModuleInterfaceDep>> m_dependencies;
    std::vector<std::string> m_supported_versions;
    const uint8_t* m_logo;
    size_t m_logo_size;

    std::string m_state = "unknow";
    std::shared_ptr<Cherry::AppWindow> m_main_window;

    std::vector<std::shared_ptr<ModuleFunction>> m_functions;
    std::vector<std::shared_ptr<ModuleOutputEvent>> m_output_events;
    std::vector<std::shared_ptr<ModuleInputEvent>> m_input_events;

private:
    std::vector<std::shared_ptr<ModuleDummyFunction>> m_dummy_functions;
    std::vector<std::shared_ptr<ModuleRenderInstance>> m_render_instances;
};

#endif // MODULE_INTERFACE_H