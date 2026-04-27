//
//  function.cpp
//  Sources for plugins functions features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <plugins/function.h>

PluginFunction::PluginFunction(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : m_function(foo),
      m_name(name) {
}

PluginFunction::PluginFunction(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      m_name(name) {
}

PluginFunction::PluginFunction(std::function<void(ReturnValues &)> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      m_name(name) {
}

PluginFunction::PluginFunction(std::function<void()> foo, const std::string &name)
    : m_function([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      m_name(name) {
}
