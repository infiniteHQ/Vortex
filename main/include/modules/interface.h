

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

class Parameters {
private:
    std::map<std::string, std::variant<int, double, std::string, void*>> params;

public:
    Parameters(std::initializer_list<std::pair<std::string, std::variant<int, double, std::string, void*>>> initList) {
        for (const auto& pair : initList) {
            params.insert(pair);
        }
    }

    Parameters(){};

    template<typename T>
    void setParameter(const std::string& key, const T& value) {
        params[key] = value;
    }

    template<typename T>
    T getParameter(const std::string& key) const {
        auto it = params.find(key);
        if (it != params.end()) {
            try {
                return std::get<T>(it->second);
            } catch (const std::bad_variant_access&) {
                throw std::runtime_error("Param type isn't valid");
            }
        } else {
            throw std::out_of_range("Key doesn't exist");
        }
    }

    void displayParameters() const {
        for (const auto& pair : params) {
            std::visit([](const auto& value) {
                std::cout << value << ' ';
            }, pair.second);
            std::cout << std::endl;
        }
    }
};


class ModuleFunction
{
public:
    ModuleFunction(void(*foo)(), const std::string& name) {this->m_foo = foo; this->m_name = name;};
    //ModuleFunction(void(*foo)(), const std::string& name) {this->m_foo = foo; this->m_name = name;};

    virtual void execute() {};


    void(*m_foo)();
    std::string m_name;
    std::shared_ptr<Parameters> m_params;
    std::vector<std::string> m_params_def;
};


/**
 * @brief Executed by other with custom parameters
 */
class ModuleInputEvent
{
public:

    ModuleInputEvent(void(*foo)(std::shared_ptr<hArgs> args), const std::string& name) {this->m_foo = foo; this->m_name = name;};
    
    virtual void execute(){};

    void(*m_foo)(std::shared_ptr<hArgs> args);
    std::string m_name;
};

/**
 * @brief Emmiting this even with custom parameters.
 */
class ModuleOutputEvent
{
public:
    ModuleOutputEvent(void(*foo)(std::shared_ptr<hArgs> args), const std::string& name) {this->m_foo = foo; this->m_name = name;};
    virtual void execute() {};

    void(*m_foo)(std::shared_ptr<hArgs> args);
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
    virtual void execute()  = 0;
    virtual void render()   = 0;

    void OnInputEvent();
    void OnOutputEvent();
    
    void AddInputEventHandler();
    void AddOutputEventHandler();

    template<typename T>
    void AddArg(const std::string& key, T value)
    {
        if(this->m_args == NULL){
            std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
            this->m_args = args;
            this->m_args->add<T>(key.c_str(), value);
        }
        else{
            this->m_args->add<T>(key.c_str(), value);
        }

    }

    void AddLogo(const uint8_t* hexa, size_t size)
    {
        this->m_logo = hexa;
        this->m_logo_size = size;
    }

    void AddLogo(const std::string& relative_path)
    {
    }

    // Functions methods
    void AddFunction(void (*item)(), const std::string& name)
    {
        std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(item, name);
        this->m_functions.push_back(p_function);
    }
/*
    void AddFunction(void (*item)(), const std::string& name)
    {
        std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(item, name);
        this->m_functions.push_back(p_function);
    }*/

    void AddFunction(void (*item)(), const std::string& name, Parameters params)
    {
        std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(item, name);
        p_function->m_params = std::make_shared<Parameters>(params);
        this->m_functions.push_back(p_function);
    }


    /**
     * @brief Other elements can exec event that added here
    */
    void AddInputEvent(void (*item)(std::shared_ptr<hArgs> args), const std::string& name)
    {
        std::shared_ptr<ModuleInputEvent> p_event = std::make_shared<ModuleInputEvent>(item, name);
        this->m_input_events.push_back(p_event);
    }

    /**
     * @brief When other elements, modules or plugin deploy a event indireclty, function that added here can handle this remote event 
    */
    void AddOutputEvent(void (*item)(std::shared_ptr<hArgs> args), const std::string& name)
    {
        std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(item, name);
        this->m_output_events.push_back(p_event);
    }

    // GetReturn get in args "FunctionName.params.return"
    // GetReturn get in args "InputEvent.params.return"

    // TODO : Creer des events et les assigner a des id unqiues

    void ExecFunction(const std::string& name){
        for(auto foo : this->m_functions){
            if(foo->m_name == name){
                foo->m_foo();
            }
        }
    }

    void ExecInputEvent(const std::string& name, std::shared_ptr<hArgs> args){
        for(auto event : this->m_input_events){
            if(event->m_name == name){
                event->m_foo(args);
            }
        }
    }
    
    void ExecOutputEvent(const std::string& name, std::shared_ptr<hArgs> args){
        for(auto event : this->m_output_events){
            if(event->m_name == name){
                event->m_foo(args);
            }
        }
    }

    template<typename T>
    void AddModuleItemParam(const void *item, std::pair<std::string, T> parameter);
    
    void AddModuleOutputEvent(const ModuleOutputEvent& event);
    void AddModuleInputEvent(const ModuleInputEvent& event);
    void AddModuleRenderInstance(const ModuleRenderInstance& event);
    void AddModuleFunction(const ModuleFunction& event);
    std::vector<std::shared_ptr<ModuleRenderInstance>> GetModuleRenderInstances() {return this->m_render_instances;};

    std::shared_ptr<hArgs> m_args;
public:
    std::string m_type;
    std::string m_proper_name;
    std::string m_name;
    std::string m_version;
    std::string m_author;
    std::string m_group;
    std::string m_picture;
    std::string m_description;
    bool        m_auto_exec;
    std::vector<std::string> m_contributors;
    const uint8_t* m_logo;
    size_t m_logo_size;
    //VxContext* m_ctx;

    // std::shared_ptr<Walnut::Image> m_module_icon;
    std::vector<std::shared_ptr<ModuleFunction>> m_functions;
    std::vector<std::shared_ptr<ModuleOutputEvent>> m_output_events;
    std::vector<std::shared_ptr<ModuleInputEvent>> m_input_events;

private:
    

    std::vector<std::shared_ptr<ModuleDummyFunction>> m_dummy_functions;
    std::vector<std::shared_ptr<ModuleRenderInstance>> m_render_instances;

};

#endif // MODULE_INTERFACE_H