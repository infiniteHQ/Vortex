#pragma once
#include <vortex.h>

#ifndef VORTEX_CREDITS_HPP
#define VORTEX_CREDITS_HPP

namespace vxe {
  VORTEX_API void add_credits(const std::string &topic, const std::string &credit_file);
  VORTEX_API void set_credits_file(const std::string &topic, const std::string &credit_file);
}  // namespace vxe

#endif  // VORTEX_CREDITS_HPP