//
//  dummy_function.cpp
//  Sources for modules dummy functions features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../include/modules/dummy_function.hpp"

/**
 * @brief Constructor for ModuleDummyFunction.
 *
 * @param foo Pointer to the function.
 * @param name Name of the function.
 */
ModuleDummyFunction::ModuleDummyFunction(void (*foo)(), const std::string& name) : m_foo(foo), m_name(name) {
}

/**
 * @brief Gets the name of the function from its pointer.
 *
 * @tparam T Type of the function pointer.
 * @param functionPtr Pointer to the function.
 * @return The name of the function.
 */
template<typename T>
std::string ModuleDummyFunction::get_function_name(T* functionPtr) {
  typedef typename std::remove_pointer<T>::type FunctionType;
  const char* name = typeid(FunctionType).name();
  return std::string(name);
}