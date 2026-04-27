//
//  random.hpp
//  Function headers for generating random values
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#ifndef VORTEX_RANDOM_HPP
#define VORTEX_RANDOM_HPP

namespace vxe {
  VORTEX_API std::string gen_random(const int len);
}  // namespace vxe

#endif  // VORTEX_RANDOM_HPP