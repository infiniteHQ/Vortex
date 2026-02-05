#include <plugins/function.h>

/**
 * @brief Constructor for PluginFunction.
 *
 * @param foo <Arguments, Return> Pointer to the function.
 * @param name The name of the function.
 */
PluginFunction::PluginFunction(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name)
    : m_function(foo), m_name(name) {}

/**
 * @brief Constructor for PluginFunction.
 *
 * @param foo <Arguments> Pointer to the function.
 * @param name The name of the function.
 */
PluginFunction::PluginFunction(std::function<void(ArgumentValues &)> foo,
                               const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      m_name(name) {}
/**
 * @brief Constructor for PluginFunction.
 *
 * @param foo <Return> Pointer to the function.
 * @param name The name of the function.
 */
PluginFunction::PluginFunction(std::function<void(ReturnValues &)> foo,
                               const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      m_name(name) {}

/**
 * @brief Constructor for PluginFunction.
 *
 * @param foo Pointer to the function.
 * @param name The name of the function.
 */
PluginFunction::PluginFunction(std::function<void()> foo,
                               const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      m_name(name) {}
