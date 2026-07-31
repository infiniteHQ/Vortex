//
//  verify.cpp
//  Sources for verify folders and files on a filesystem
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API bool vxe::file_exists(const std::string &path) {
  std::error_code ec;
  return fs::exists(path, ec) && fs::is_regular_file(path, ec);
}