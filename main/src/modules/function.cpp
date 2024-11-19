#include "../../include/modules/function.h"

/**
 * @brief Constructor for ModuleFunction.
 *
 * @param foo <Arguments, Return> Pointer to the function.
 * @param name The name of the function.
 */
ModuleFunction::ModuleFunction(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : m_function(foo), m_name(name) {}

/**
 * @brief Constructor for ModuleFunction.
 *
 * @param foo <Arguments> Pointer to the function.
 * @param name The name of the function.
 */
ModuleFunction::ModuleFunction(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret)
                 {
          ret = ReturnValues(); 
          foo(args); }),
      m_name(name) {}
/**
 * @brief Constructor for ModuleFunction.
 *
 * @param foo <Return> Pointer to the function.
 * @param name The name of the function.
 */
ModuleFunction::ModuleFunction(std::function<void(ReturnValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret)
                 {
                     foo(ret);
                 }),
      m_name(name)
{
}

/**
 * @brief Constructor for ModuleFunction.
 *
 * @param foo Pointer to the function.
 * @param name The name of the function.
 */
ModuleFunction::ModuleFunction(std::function<void()> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &)
                 { foo(); }),
      m_name(name) {}
