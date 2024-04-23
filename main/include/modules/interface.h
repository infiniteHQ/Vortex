

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
#include <cxxabi.h> 
#include "../../../lib/uikit/Platform/GUI/editor/Image.h"
#include "function.h"

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

class ModuleFunction
{
public:
    ModuleFunction(void(*foo)(), const std::string& name) {this->m_foo = foo; this->m_name = name;};
    //ModuleFunction(void(*foo)(), const std::string& name) {this->m_foo = foo; this->m_name = name;};

    virtual void execute() {};


    void(*m_foo)();
    std::string m_name;
    //std::shared_ptr<Parameters> m_params;
    std::vector<std::string> m_params_def;
};


/**
 * @brief Executed by other with custom parameters
 */
class ModuleInputEvent
{
public:

    ModuleInputEvent(void(*foo)(const std::shared_ptr<hArgs>& args), const std::string& name) {this->m_foo = foo; this->m_name = name;};
    
    virtual void execute(){};

    void(*m_foo)(const std::shared_ptr<hArgs>& args);
    std::string m_name;
};

/**
 * @brief Emmiting this even with custom parameters.
 */
class ModuleOutputEvent
{
public:
    ModuleOutputEvent(void(*foo)(const std::shared_ptr<hArgs>& args), const std::string& name) {this->m_foo = foo; this->m_name = name;};
    virtual void execute() {};

    void(*m_foo)(const std::shared_ptr<hArgs>& args);
    std::string m_name;
};



class ModuleDummyFunction
{
public:
    ModuleDummyFunction(void(*foo)(), const std::string& name) {this->m_foo = foo; this->m_name = name;};
    virtual void execute() {};

    template<typename T>
    std::string GetFunctionName(T* functionPtr) {
    typedef typename std::remove_pointer<T>::type FunctionType;
    const char* name = typeid(FunctionType).name();
    return name;
}
    std::string m_name;
    void(*m_foo)();
};




class ModuleRenderInstance
{
public:
    virtual void render() {};
};

class ModuleInterface
{
    // Module API (declarations needs to be here)
public:
    virtual ~ModuleInterface() {}
    virtual void execute()  {};
    virtual void render()   {};

    void OnInputEvent();
    void OnOutputEvent();
    
    void AddInputEventHandler();
    void AddOutputEventHandler();

    template<typename T>
    void AddArg(const std::string& key, T value);

    void AddLogo(const uint8_t* hexa, size_t size);
    void AddLogo(const std::string& relative_path);
    void AddFunction(void (*item)(), const std::string& name);

    //void AddFunction(void (*item)(), const std::string& name, Parameters params);
    void AddInputEvent(void (*item)(const std::shared_ptr<hArgs>& args), const std::string& name);
    void AddOutputEvent(void (*item)(const std::shared_ptr<hArgs>& args), const std::string& name);
    std::shared_ptr<ModuleInterface> GetInterface();

    void ExecFunction(const std::string& name);
    void ExecInputEvent(const std::string& name, std::shared_ptr<hArgs> args);
    void ExecOutputEvent(const std::string& name, std::shared_ptr<hArgs> args);

    template<typename T>
    void AddModuleItemParam(const void *item, std::pair<std::string, T> parameter);
    
    void AddModuleOutputEvent(const ModuleOutputEvent& event);
    void AddModuleInputEvent(const ModuleInputEvent& event);
    void AddModuleRenderInstance(const std::shared_ptr<ModuleRenderInstance> &event);
    void AddModuleFunction(const ModuleFunction& event);
    std::vector<std::shared_ptr<ModuleRenderInstance>> GetModuleRenderInstances() {return this->m_render_instances;};

    void LogInfo(const std::string& message);
    void LogWarning(const std::string& message);
    void LogError(const std::string& message);
    void LogFatal(const std::string& message);

    static std::shared_ptr<ModuleInterface> GetEditorModuleByName(const std::string& name);



    std::shared_ptr<hArgs> m_args;
    std::string m_datapath;
public:
    std::string m_type;
    std::string m_proper_name;
    std::string m_name;
    std::string m_version;
    std::string m_author;
    std::string m_group;
    std::string m_picture;
    std::string m_logo_path;
    std::string m_description;
    bool        m_auto_exec;
    std::vector<std::string> m_contributors;
    const uint8_t* m_logo;
    size_t m_logo_size;

    std::vector<std::shared_ptr<ModuleFunction>> m_functions;
    std::vector<std::shared_ptr<ModuleOutputEvent>> m_output_events;
    std::vector<std::shared_ptr<ModuleInputEvent>> m_input_events;

private:
    std::vector<std::shared_ptr<ModuleDummyFunction>> m_dummy_functions;
    std::vector<std::shared_ptr<ModuleRenderInstance>> m_render_instances;

};

#endif // MODULE_INTERFACE_H