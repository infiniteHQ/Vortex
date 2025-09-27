#include "../../include/modules/load.h"
#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

#ifdef _WIN32
#include <stringapiset.h>
#include <windows.h>

std::wstring ConvertToWideString(const std::string &str) {
  int size_needed =
      MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
  if (size_needed == 0) {
    throw std::runtime_error("Error converting string to wide string");
  }
  std::wstring wide_string(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wide_string[0],
                      size_needed);
  return wide_string;
}

std::string ConvertToString(const std::wstring &wstr) {
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr,
                                        0, nullptr, nullptr);
  if (size_needed == 0) {
    throw std::runtime_error("Error converting wide string to string");
  }
  std::string str(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed,
                      nullptr, nullptr);
  return str;
}
#endif

namespace VortexMaker {

VORTEX_API void
LoadEditorModules(const std::string &directory,
                  std::vector<void *> &modules_handlers,
                  std::vector<std::shared_ptr<ModuleInterface>> &modules) {
  modules.clear();
  modules_handlers.clear();

  auto module_files = VortexMaker::SearchFiles(directory, "module.json");

  for (const auto &file : module_files) {
    try {
      auto json_data = VortexMaker::DumpJSON(file);

      fs::path module_path(file);
      fs::path path = module_path.parent_path();

#ifdef _WIN32
      auto so_files = VortexMaker::SearchFiles(path.string(), "module.dll");
#else
      auto so_files = VortexMaker::SearchFiles(path.string(), "libmodule.so");
#endif

      for (auto &s : so_files)
        std::cout << "  " << s << "\n";

      for (const auto &so_file : so_files) {
        void *handle = nullptr;

#ifdef _WIN32
        std::wstring wide_so_file = ConvertToWideString(so_file);

        handle = LoadLibraryW(wide_so_file.c_str());
        if (!handle) {
          DWORD error_code = GetLastError();
          LPVOID msg_buffer;

          FormatMessageW(
              FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
              nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
              (LPWSTR)&msg_buffer, 0, nullptr);

          std::wstring wide_error_msg = (LPWSTR)msg_buffer;
          LocalFree(msg_buffer);

          std::string error_msg = ConvertToString(wide_error_msg);
          VortexMaker::LogFatal("Modules",
                                "Failed to load module: " + error_msg);
          continue;
        }

        auto create_em = reinterpret_cast<ModuleInterface *(*)()>(
            GetProcAddress((HMODULE)handle, "create_em"));
        if (!create_em) {
          DWORD error_code = GetLastError();
          LPVOID msg_buffer;

          FormatMessageW(
              FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
              nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
              (LPWSTR)&msg_buffer, 0, nullptr);

          std::wstring wide_error_msg = (LPWSTR)msg_buffer;
          LocalFree(msg_buffer);

          std::string error_msg = ConvertToString(wide_error_msg);
          VortexMaker::LogFatal("Modules",
                                "Failed to load symbol: " + error_msg);
          FreeLibrary((HMODULE)handle);
          continue;
        }
#else
        handle = dlopen(so_file.c_str(), RTLD_LAZY | RTLD_GLOBAL);
        if (!handle) {
          std::string error_msg = dlerror();
          VortexMaker::LogFatal("Modules",
                                "Failed to load module: " + error_msg);
          continue;
        }

        auto create_em = reinterpret_cast<ModuleInterface *(*)()>(
            dlsym(handle, "create_em"));
        const char *dlsym_error = dlerror();
        if (dlsym_error) {
          std::string error_msg = dlsym_error;
          VortexMaker::LogFatal("Modules",
                                "Failed to load symbol: " + error_msg);
          dlclose(handle);
          continue;
        }
#endif

        std::shared_ptr<ModuleInterface> new_module(create_em());
        try {
          new_module->m_name = json_data["name"].get<std::string>();
          new_module->m_auto_exec = json_data["auto_exec"].get<bool>();
          new_module->m_proper_name =
              json_data["proper_name"].get<std::string>();
          new_module->m_type = json_data["type"].get<std::string>();
          new_module->m_version = json_data["version"].get<std::string>();
          new_module->m_description =
              json_data["description"].get<std::string>();
          new_module->m_picture = json_data["picture"].get<std::string>();
          new_module->m_logo_path =
              (fs::path(path) / new_module->m_picture).string();
          new_module->m_path = (fs::path(path)).string();
          new_module->m_binary_path =
              (fs::path(so_file).parent_path()).string();

          new_module->m_author = json_data["author"].get<std::string>();
          new_module->m_group = json_data["group"].get<std::string>();
          new_module->m_contributors =
              json_data["contributors"].get<std::vector<std::string>>();

          for (auto dep : json_data["deps"]) {
            std::shared_ptr<ModuleInterfaceDep> dependence =
                std::make_shared<ModuleInterfaceDep>();
            dependence->name = dep["name"].get<std::string>();
            dependence->type = dep["type"].get<std::string>();
            for (auto version : dep["versions"]) {
              dependence->supported_versions.push_back(version);
            }
            new_module->m_dependencies.push_back(dependence);
          }

          new_module->m_supported_versions =
              json_data["compatibility"].get<std::vector<std::string>>();
        } catch (const std::exception &e) {
          VortexMaker::LogError("Modules", "Failed to load module metadata: " +
                                               std::string(e.what()));
        }

        modules.push_back(new_module);
        modules_handlers.push_back(handle);
      }
    } catch (const std::exception &e) {
      VortexMaker::LogError("Modules", "Error processing module file: " +
                                           std::string(e.what()));
    }
  }
}

VORTEX_API void
LoadSystemModules(std::vector<std::shared_ptr<ModuleInterface>> &sys_modules) {
  // Get the home directory
  std::string homeDir = VortexMaker::getHomeDirectory();

  // Module path on the system
  std::string path = homeDir + "/.vx/modules";

  // Search for module files recursively in the directory
  auto module_files = VortexMaker::SearchSystemFiles(path, "module.json");

  // Clear system modules vector
  sys_modules.clear();

  // Iterate through each found module file
  for (const auto &file : module_files) {
    try {
      // Load JSON data from the module configuration file
      auto json_data = VortexMaker::DumpJSON(file);

      std::string module_path = file.substr(0, file.find_last_of("/"));

      std::shared_ptr<ModuleInterface> new_module =
          std::make_shared<ModuleInterface>();

      // Try to fetch module informations from module.json
      try {
        new_module->m_name = json_data["name"].get<std::string>();
        new_module->m_auto_exec = json_data["auto_exec"].get<bool>();
        new_module->m_proper_name = json_data["proper_name"].get<std::string>();
        new_module->m_type = json_data["type"].get<std::string>();
        new_module->m_version = json_data["version"].get<std::string>();
        new_module->m_description = json_data["description"].get<std::string>();
        new_module->m_picture = json_data["picture"].get<std::string>();
        new_module->m_logo_path = module_path + "/" + new_module->m_picture;
        new_module->m_path = module_path + "/";
        new_module->m_author = json_data["author"].get<std::string>();
        new_module->m_group = json_data["group"].get<std::string>();
        new_module->m_contributors =
            json_data["contributors"].get<std::vector<std::string>>();

        for (auto dep : json_data["deps"]) {
          std::shared_ptr<ModuleInterfaceDep> dependence =
              std::make_shared<ModuleInterfaceDep>();
          dependence->name = dep["name"].get<std::string>();
          dependence->type = dep["type"].get<std::string>();
          for (auto version : dep["versions"]) {
            dependence->supported_versions.push_back(version);
          }
          new_module->m_dependencies.push_back(dependence);
        }

        new_module->m_supported_versions =
            json_data["compatibility"].get<std::vector<std::string>>();
      } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
      }

      VXINFO("Modules registered in system ",
             new_module->m_name + " loaded with success !")

      // Store the module instance and handle
      sys_modules.push_back(new_module);
    } catch (const std::exception &e) {
      // Print error if an exception occurs
      VortexMaker::LogError("Core", std::string("Error: ") + e.what());
    }
  }
}

} // namespace VortexMaker