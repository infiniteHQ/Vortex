#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_FUNCTION_H
#define PLUGIN_FUNCTION_H

// TODO : Replace all args by json values (params like return)

/*
Functions models :

   Foo        Params           Return
Function [JSONPARAMETERS]   [JSONRETURN]
Function [void]             [JSONRETURN]
Function [JSONPARAMETERS]   [void]
Function [void]             [void]
*/

/**
 * @brief Scopes of a PluginFunction. If PUBLIC, the function can be called by
 * every other members of the project (plugins, modules, etc...). If PRIVATE,
 * the function can only be called by itself. If RESTRICTED, the function can be
 * called by linked members (child plugins/modules, optionnal extentions of the
 * module/plugin)
 */
enum class PluginFunctionScope { PRIVATE, PUBLIC, RESTRICTED };

class PluginFunction {
public:
  PluginFunction(std::function<void(ArgumentValues &, ReturnValues &)> foo,
                 const std::string &name);
  PluginFunction(std::function<void(ArgumentValues &)> foo,
                 const std::string &name);
  PluginFunction(std::function<void(ReturnValues &)> foo,
                 const std::string &name);
  PluginFunction(std::function<void()> foo, const std::string &name);

  virtual void execute() {};

  std::function<void(ArgumentValues &, ReturnValues &)> m_function;
  ArgumentValues m_return_values;
  PluginFunctionScope m_scope;

  std::string m_name;
  std::string m_description;
  bool m_can_callback;
  std::shared_ptr<hArgs> m_args;
  std::vector<std::tuple<std::string, std::string, std::string>> m_params_def;
};

#endif