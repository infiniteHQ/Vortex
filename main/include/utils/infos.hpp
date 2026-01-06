#pragma once
#include "../vortex.h"

#ifndef VORTEX_INFOS_HPP
#define VORTEX_INFOS_HPP

#ifdef _WIN32
#else
#include <openssl/sha.h>
#endif

namespace VortexMaker {
std::string GetBuildDate();
std::string GetGitCommit();
std::string GetVortexEditorDist();
std::string GetVortexBuildID();
std::string GetVortexBuildName();
} // namespace VortexMaker

#ifdef _WIN32
#define VORTEX_EXECUTABLE "vortex_launcher.exe"
#else
#define VORTEX_EXECUTABLE "vortex_launcher"
#endif

static std::string vxl_exehash = "undefined";
static std::string system_desktop = "undefined";

#ifdef _WIN32
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <wincrypt.h>
#include <windows.h>

#pragma comment(lib, "advapi32.lib")

namespace VortexMaker {
std::string ComputeSHA256Short(const std::string &filepath, size_t length = 10);
}
#else

namespace VortexMaker {
std::string ComputeSHA256Short(const std::string &filepath, size_t length = 10);
} // namespace VortexMaker
#endif

namespace VortexMaker {
std::string GetVortexEditorHash();
}

#if defined(_WIN32)

namespace VortexMaker {
std::string GetWindowsVersion();
} // namespace VortexMaker

#define OS_NAME GetWindowsVersion().c_str()
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_NAME "macOS"
#elif defined(__linux__)
namespace VortexMaker {
std::string GetLinuxDistroName();
std::string GetLinuxDesktopEnvAndDisplayServer();
} // namespace VortexMaker
#define OS_NAME VortexMaker::GetLinuxDistroName().c_str()
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

#endif // VORTEX_INFOS_HPP