//
//  function.cpp
//  Sources for plugins functions features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <plugins/function.hpp>

PluginFunction::PluginFunction(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name)
    : function_(foo),
      name_(name) {
}

PluginFunction::PluginFunction(std::function<void(ArgumentValues &)> foo, const std::string &name)
    : function_([foo](ArgumentValues &args, ReturnValues &ret) {
        ret = ReturnValues();
        foo(args);
      }),
      name_(name) {
}

PluginFunction::PluginFunction(std::function<void(ReturnValues &)> foo, const std::string &name)
    : function_([foo](ArgumentValues &, ReturnValues &ret) { foo(ret); }),
      name_(name) {
}

PluginFunction::PluginFunction(std::function<void()> foo, const std::string &name)
    : function_([foo](ArgumentValues &, ReturnValues &) { foo(); }),
      name_(name) {
}
