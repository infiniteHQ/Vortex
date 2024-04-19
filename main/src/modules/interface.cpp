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

std::shared_ptr<ModuleInterface> ModuleInterface::GetEditorModuleByName(const std::string& name){
    VxContext *ctx = VortexMaker::GetCurrentContext();
    for(auto em : ctx->IO.em){
        if(name == em->m_name){
            return em;
        }
    }
    return nullptr;
}