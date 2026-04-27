//
//  dummy_function.hpp
//  Headers and utilities for modules dummy functions
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef MODULE_DUMMYFUNCTION_H
#define MODULE_DUMMYFUNCTION_H

class ModuleDummyFunction {
 public:
  ModuleDummyFunction(void (*foo)(), const std::string& name);
  virtual void execute() { };

  template<typename T>
  std::string get_function_name(T* functionPtr);

  std::string m_name;
  void (*m_foo)();
};

#endif