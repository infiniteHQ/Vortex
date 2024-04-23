#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"
#include "../../include/modules/interface.h"

void ModuleInterface::AddModuleFunction(const ModuleFunction &function)
{
    std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(function);
    this->m_functions.push_back(p_function);
}

/*
void ModuleInterface::AddFunction(void (*item)())
{
    std::shared_ptr<ModuleDummyFunction> p_function = std::make_shared<ModuleDummyFunction>(item);
    this->m_dummy_functions.push_back(p_function);
}*/

void ModuleInterface::AddModuleOutputEvent(const ModuleOutputEvent &event)
{
    std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(event);
    this->m_output_events.push_back(p_event);
}

void ModuleInterface::AddModuleInputEvent(const ModuleInputEvent &event)
{
}

void ModuleInterface::AddModuleRenderInstance(const std::shared_ptr<ModuleRenderInstance> &event)
{
    // std::shared_ptr<ModuleRenderInstance> p_event = std::make_shared<ModuleRenderInstance>(event);
    this->m_render_instances.push_back(event);
}

template <typename T>
void ModuleInterface::AddArg(const std::string &key, T value)
{
    if (this->m_args == NULL)
    {
        std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
        this->m_args = args;
        this->m_args->add<T>(key.c_str(), value);
    }
    else
    {
        this->m_args->add<T>(key.c_str(), value);
    }
}

void ModuleInterface::AddLogo(const uint8_t *hexa, size_t size)
{
    this->m_logo = hexa;
    this->m_logo_size = size;
}

void ModuleInterface::AddLogo(const std::string &relative_path)
{
}

// Functions methods
void ModuleInterface::AddFunction(void (*item)(), const std::string &name)
{
    std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(item, name);
    this->m_functions.push_back(p_function);
}

/**
 * @brief Other elements can exec event that added here
 */
void ModuleInterface::AddInputEvent(void (*item)(const std::shared_ptr<hArgs> &args), const std::string &name)
{
    std::shared_ptr<ModuleInputEvent> p_event = std::make_shared<ModuleInputEvent>(item, name);
    this->m_input_events.push_back(p_event);
}

/**
 * @brief When other elements, modules or plugin deploy a event indireclty, function that added here can handle this remote event
 */
void ModuleInterface::AddOutputEvent(void (*item)(const std::shared_ptr<hArgs> &args), const std::string &name)
{
    std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(item, name);
    this->m_output_events.push_back(p_event);
}

std::shared_ptr<ModuleInterface> ModuleInterface::GetInterface()
{
    return std::make_shared<ModuleInterface>(*this);
}

std::shared_ptr<ModuleInterface> ModuleInterface::GetEditorModuleByName(const std::string &name)
{
    VxContext *ctx = VortexMaker::GetCurrentContext();
    for (auto em : ctx->IO.em)
    {
        if (name == em->m_name)
        {
            return em;
        }
    }
    return nullptr;
}

void ModuleInterface::ExecFunction(const std::string &name)
{
    for (auto foo : this->m_functions)
    {
        if (foo->m_name == name)
        {
            foo->m_foo();
        }
    }
}

void ModuleInterface::ExecInputEvent(const std::string &name, std::shared_ptr<hArgs> args)
{
    for (auto event : this->m_input_events)
    {
        if (event->m_name == name)
        {
            event->m_foo(args);
        }
    }
}

void ModuleInterface::ExecOutputEvent(const std::string &name, std::shared_ptr<hArgs> args)
{
    for (auto event : this->m_output_events)
    {
        if (event->m_name == name)
        {
            event->m_foo(args);
        }
    }
}