//
//  dummy_function.cpp
//  Sources for plugins dummy functions
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <plugins/dummy_function.h>

PluginDummyFunction::PluginDummyFunction(void (*foo)(), const std::string &name) : m_foo(foo), m_name(name) {
}

template<typename T>
std::string PluginDummyFunction::get_function_name(T *functionPtr) {
  typedef typename std::remove_pointer<T>::type FunctionType;
  const char *name = typeid(FunctionType).name();
  return std::string(name);
}