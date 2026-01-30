#include "../include/utils/infos.hpp"

std::string VortexMaker::GetBuildDate() { return BUILD_DATE_STR; }

std::string VortexMaker::GetGitCommit() { return GIT_COMMIT_HASH; }

std::string VortexMaker::GetVortexEditorDist() { return BUILD_DIST; }

std::string VortexMaker::GetVortexBuildID() { return VORTEX_BUILDID; }

std::string VortexMaker::GetVortexBuildName() { return VORTEX_BUILDNAME; }

#ifdef _WIN32
std::string VortexMaker::ComputeSHA256Short(const std::string &filepath,
                                            size_t length) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file)
    return "undefined";

  std::vector<char> buffer(8192);
  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;
  BYTE hash[32]; // SHA-256 produces 32-byte hash
  DWORD hashLen = sizeof(hash);

  if (!CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_AES,
                           CRYPT_VERIFYCONTEXT)) {
    return "undefined";
  }

  if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
    CryptReleaseContext(hProv, 0);
    return "undefined";
  }

  while (file.read(buffer.data(), buffer.size()) || file.gcount()) {
    if (!CryptHashData(hHash, reinterpret_cast<BYTE *>(buffer.data()),
                       static_cast<DWORD>(file.gcount()), 0)) {
      CryptDestroyHash(hHash);
      CryptReleaseContext(hProv, 0);
      return "undefined";
    }
  }

  if (!CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0)) {
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return "undefined";
  }

  CryptDestroyHash(hHash);
  CryptReleaseContext(hProv, 0);

  std::ostringstream oss;
  for (DWORD i = 0; i < hashLen && oss.tellp() < (std::streampos)length * 2;
       ++i) {
    oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }

  std::string result = oss.str();
  return result.substr(0, length);
}
#else
std::string VortexMaker::ComputeSHA256Short(const std::string &filepath,
                                            size_t length) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file)
    return "undefined";

  SHA256_CTX sha256;
  SHA256_Init(&sha256);

  std::vector<char> buffer(8192);
  while (file.read(buffer.data(), buffer.size()) || file.gcount()) {
    SHA256_Update(&sha256, buffer.data(), file.gcount());
  }

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_Final(hash, &sha256);

  std::ostringstream oss;
  for (size_t i = 0; i < SHA256_DIGEST_LENGTH && oss.tellp() < length; ++i) {
    oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }

  std::string result = oss.str();
  return result.substr(0, length);
}
#endif

std::string VortexMaker::GetVortexEditorHash() {
  if (vxl_exehash.empty()) {
    std::string exename = VORTEX_EXECUTABLE;
    std::string exepath = Cherry::GetPath(exename);
    if (std::filesystem::exists(exepath)) {
      vxl_exehash = ComputeSHA256Short(exepath);
    }
  }
  return vxl_exehash;
}

#if defined(_WIN32)
std::string VortexMaker::GetWindowsVersion() {
  HKEY hKey;
  LONG lRes = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                            0, KEY_READ, &hKey);

  if (lRes != ERROR_SUCCESS)
    return "Unknown Windows version";

  char productName[256];
  DWORD size = sizeof(productName);
  RegQueryValueExA(hKey, "ProductName", NULL, NULL, (LPBYTE)productName, &size);

  DWORD buildNumber = 0;
  size = sizeof(buildNumber);
  RegQueryValueExA(hKey, "CurrentBuildNumber", NULL, NULL, (LPBYTE)&buildNumber,
                   &size);

  char releaseId[256] = {0};
  size = sizeof(releaseId);
  RegQueryValueExA(hKey, "DisplayVersion", NULL, NULL, (LPBYTE)releaseId,
                   &size);

  RegCloseKey(hKey);

  std::string versionStr =
      std::string(productName) + " " + std::string(releaseId);
  return versionStr;
}

#define OS_NAME GetWindowsVersion().c_str()
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_NAME "macOS"
#elif defined(__linux__)
std::string VortexMaker::GetLinuxDistroName() {
  std::ifstream file("/etc/os-release");
  std::string line;
  std::string distro = "Linux";

  while (std::getline(file, line)) {
    if (line.rfind("PRETTY_NAME=", 0) == 0) {
      std::string value = line.substr(
          13, line.size() - 14); // remove PRETTY_NAME=" and ending "
      distro = "Linux (" + value + ")";
      break;
    }
  }

  return distro;
}
std::string VortexMaker::GetLinuxDesktopEnvAndDisplayServer() {
  const char *xdgDesktop = std::getenv("XDG_CURRENT_DESKTOP");
  const char *desktopSession = std::getenv("DESKTOP_SESSION");
  const char *waylandDisplay = std::getenv("WAYLAND_DISPLAY");
  const char *x11Display = std::getenv("DISPLAY");

  std::string de;
  if (xdgDesktop) {
    de = xdgDesktop;
  } else if (desktopSession) {
    de = desktopSession;
  } else {
    de = "Unknown DE";
  }

  std::string compositor;
  if (waylandDisplay) {
    compositor = "Wayland";
  } else if (x11Display) {
    compositor = "X11";
  } else {
    compositor = "Unknown Display";
  }

  return de + " " + compositor;
}

#define OS_NAME getLinuxDistroName().c_str()
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