#include "../../include/modules/event.h"

/**
 * @brief Constructor for ModuleInputEvent.
 *
 * @param foo Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param name The name of the event.
 */
ModuleInputEvent::ModuleInputEvent(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name)
    : m_foo(foo), m_name(name) {}


/**
 * @brief Constructor for ModuleOutputEvent.
 *
 * @param foo Pointer to the function taking a shared_ptr to hArgs as parameter.
 * @param name The name of the event.
 */
ModuleOutputEvent::ModuleOutputEvent(void (*foo)(const std::shared_ptr<hArgs>& args), const std::string& name)
    : m_foo(foo), m_name(name) {}