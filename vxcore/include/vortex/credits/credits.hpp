//
//  credits.hpp
//  Headers for credits features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#ifndef VORTEX_CREDITS_HPP
#define VORTEX_CREDITS_HPP

namespace vxe {
  VORTEX_API void add_credits(const std::string &topic, const std::string &credit_file);
  VORTEX_API void set_credits_file(const std::string &topic, const std::string &credit_file);
}  // namespace vxe

#endif  // VORTEX_CREDITS_HPP