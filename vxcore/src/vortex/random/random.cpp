//
//  random.cpp
//  Sources of random utilities for internal features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API std::string vxe::gen_random(int len) {
  // Alphanumeric characters to choose from
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

  // Temporary string to store the generated random string
  std::string tmp_s;
  tmp_s.reserve(len);  // Reserve space for efficiency

  // Generate random characters and append them to the string
  for (int i = 0; i < len; ++i) {
    tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  return tmp_s;  // Return the generated random string
}
