//
//  infos.hpp
//  Headers and utilities for information features. (identification of Vortex and the user environement)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../vortex.h"

#ifndef VORTEX_INFOS_HPP
#define VORTEX_INFOS_HPP

#ifdef _WIN32
#else
#include <openssl/sha.h>
#endif

namespace vxe {
  std::string get_build_date();
  std::string get_git_commit();
  std::string get_vortex_editor_dist();
  std::string get_vortex_build_id();
  std::string get_vortex_build_name();
  std::string get_vortex_build_type();
}  // namespace vxe

#ifdef _WIN32
#define VORTEX_EXECUTABLE "vortex_launcher.exe"
#else
#define VORTEX_EXECUTABLE "vortex_launcher"
#endif

static std::string vxl_exehash = "undefined";
static std::string system_desktop = "undefined";

#ifdef _WIN32
#include <wincrypt.h>
#include <windows.h>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "advapi32.lib")

namespace vxe {
  std::string compute_sha256_short(const std::string &filepath, size_t length = 10);
}
#else

namespace vxe {
  std::string compute_sha256_short(const std::string &filepath, size_t length = 10);
}  // namespace vxe
#endif

namespace vxe {
  std::string get_vortex_editor_hash();
}

#if defined(_WIN32)

namespace vxe {
  std::string get_windows_version();
}  // namespace vxe

#define OS_NAME vxe::get_windows_version().c_str()
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_NAME "macOS"
#elif defined(__linux__)
namespace vxe {
  std::string get_linux_distro_name();
  std::string get_linux_desktop_env_and_display_server();
}  // namespace vxe
#define OS_NAME vxe::get_linux_distro_name().c_str()
#else
#define OS_NAME "Unknown OS"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_NAME "x86_64"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_NAME "ARM64"
#elif defined(__i386__) || defined(_M_IX86)
#define ARCH_NAME "x86"
#else
#define ARCH_NAME "Unknown Arch"
#endif

#endif  // VORTEX_INFOS_HPP