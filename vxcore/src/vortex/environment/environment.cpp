#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::init_environment() {
  {
    std::string path = vxe::get_home_directory() + "/.vx/data/";
    vxe::create_folder_if_not_exists(path);
  }
  {
    std::string path = vxe::get_home_directory() + "/.vx/modules/";
    vxe::create_folder_if_not_exists(path);
  }

  {
    std::string path = vxe::get_home_directory() + "/.vx/templates/";
    vxe::create_folder_if_not_exists(path);
  }

  {
    std::string path = vxe::get_home_directory() + "/.vx/templates/vortex.templates.builtin.__blankproject/";
    vxe::create_folder_if_not_exists(path);
  }

  {
    std::string path = vxe::get_home_directory() + "/.vx/data/";
    std::string file = path + "projects.json";

    nlohmann::json default_data = { { "projects", nlohmann::json::array() } };

    vxe::create_file_if_not_exists(file, default_data);
  }
}

VORTEX_API void vxe::refresh_environment_projects() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  std::string path = vxe::get_home_directory() + "/.vx/data/";

  std::string json_file = path + "/projects.json";

  // Clear project list
  ctx->IO.sys_projects.clear();

  // Verify if the project is valid
  try {
    // Load JSON data from the project configuration file
    auto json_data = vxe::dump_json(json_file);
    for (auto project : json_data["projects"]) {
      std::shared_ptr<EnvProject> newproject = std::make_shared<EnvProject>();

      newproject->name = project["name"].get<std::string>();
      newproject->version = project["version"].get<std::string>();
      newproject->description = project["description"].get<std::string>();
      newproject->compatibleWith = project["compatibleWith"].get<std::string>();
      newproject->path = project["path"].get<std::string>();
      newproject->logoPath = project["logoPath"].get<std::string>();
      newproject->lastOpened = project["lastOpened"].get<std::string>();

      ctx->IO.sys_projects.push_back(newproject);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    vxe::log_error("Error: ", e.what());
  }
}

VORTEX_API void vxe::initialize_platform_vendor() {
#if defined(__linux__)
  vxe::get_current_context()->m_PlatformVendor = PlatformVendor::Linux;
#elif defined(_WIN32) || defined(_WIN64)
  vxe::get_current_context()->m_PlatformVendor = PlatformVendor::Windows;
#elif defined(__APPLE__)
  vxe::get_current_context()->m_PlatformVendor = PlatformVendor::Macos;
#else
  //
#endif
}

VORTEX_API bool vxe::is_linux() {
  return vxe::get_current_context()->m_PlatformVendor == PlatformVendor::Linux;
}

VORTEX_API bool vxe::is_not_linux() {
  return !is_linux();
}

VORTEX_API bool vxe::is_windows() {
  return vxe::get_current_context()->m_PlatformVendor == PlatformVendor::Windows;
}

VORTEX_API bool vxe::is_not_windows() {
  return !is_windows();
}

VORTEX_API bool vxe::is_macos() {
  return vxe::get_current_context()->m_PlatformVendor == PlatformVendor::Macos;
}

VORTEX_API bool vxe::is_not_macos() {
  return !is_macos();
}

VORTEX_API void vxe::refresh_environment_vortex_versions_pools() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  std::string path = vxe::get_home_directory() + "/.vx/configs/";

  std::string json_file = path + "/vortex_versions_pools.json";

  // Clear project list
  ctx->IO.sys_vortex_versions_pools.clear();

  // Verify if the project is valid
  try {
    // Load JSON data from the project configuration file
    auto json_data = vxe::dump_json(json_file);
    for (auto pool : json_data["vortex_versions_pools"]) {
      ctx->IO.sys_vortex_versions_pools.push_back(pool);
    }
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    vxe::log_error("Error: ", e.what());
  }
}

void vxe::detect_platform() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

#if defined(_WIN32) || defined(_WIN64)
  ctx->platform = "windows";
#elif defined(__APPLE__) && defined(__MACH__)
  ctx->platform = "macos";
#elif defined(__linux__)
  ctx->platform = "linux";
#elif defined(__FreeBSD__)
  ctx->platform = "freebsd";
#else
  ctx->platform = "unknown";
#endif
}

void vxe::detect_arch() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

#if defined(__x86_64__) || defined(_M_X64)
  ctx->arch = "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
  ctx->arch = "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
  ctx->arch = "arm64";
#elif defined(__arm__) || defined(_M_ARM)
  ctx->arch = "arm";
#elif defined(__riscv)
  ctx->arch = "riscv";
#elif defined(__ppc64__)
  ctx->arch = "ppc64";
#else
  ctx->arch = "unknown";
#endif
}

VORTEX_API void vxe::update_environment_project(
    const std::string &name,
    const std::string &author,
    const std::string &version,
    const std::string &compatibleWith,
    const std::string &description,
    const std::string &path,
    const std::string &logo_path,
    const std::string &template_name) {  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  std::string sys_path = vxe::get_home_directory() + "/.vx/data/";
  std::string json_file = sys_path + "/projects.json";

  // Verify if the project is valid
  // Load JSON data from the project configuration file
  auto json_data = vxe::dump_json(json_file);

  vxe::log_info("Core", "Verify if the new project \"" + name + "\" is not already existing.");

  // Check if a project with the given name exists
  bool projectExists = false;
  for (auto &project : json_data["projects"]) {
    if (project["name"].get<std::string>() == name) {
      projectExists = true;
      return;
    }
  }

  vxe::log_info("Core", "Create a new entry in registered projects for \"" + name + "\".");

  // If the project doesn't exist, create a new JSON object and add it to the
  // list
  json_data["projects"].push_back(
      { { "name", name },
        { "version", version },
        { "description", description },
        { "path", path },
        { "lastOpened", vxe::get_current_timestamp() },
        { "compatibleWith", compatibleWith },
        { "logoPath", logo_path } });

  // Write the updated JSON data back to the file
  std::ofstream output(json_file);
  output << json_data.dump(4);  // Use pretty print with indentation of 4 spaces
  output.close();
}

VORTEX_API void vxe::update_environment_project() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  std::string path = vxe::get_home_directory() + "/.vx/data/";
  std::string json_file = path + "/projects.json";

  // Verify if the project is valid
  try {
    // Load JSON data from the project configuration file
    auto json_data = vxe::dump_json(json_file);

    std::string name = ctx->name;

    // Check if a project with the given name exists
    bool projectExists = false;
    for (auto &project : json_data["projects"]) {
      if (project["name"].get<std::string>() == name) {
        // Project with the given name exists, update its information
        project["version"] = ctx->project_version;
        project["description"] = ctx->description;
        project["path"] = ctx->projectPath;
        project["compatibleWith"] = ctx->compatibleWith;
        project["lastOpened"] = vxe::get_current_timestamp();
        project["logoPath"] = ctx->logoPath;
        projectExists = true;
        break;
      }
    }

    // If the project doesn't exist, create a new JSON object and add it to the
    // list
    if (!projectExists) {
      json_data["projects"].push_back(
          { { "name", ctx->name },
            { "version", ctx->project_version },
            { "description", ctx->description },
            { "path", ctx->projectPath },
            { "lastOpened", vxe::get_current_timestamp() },
            { "compatibleWith", ctx->compatibleWith },
            { "logoPath", ctx->logoPath } });
    }

    // Write the updated JSON data back to the file
    std::ofstream output(json_file);
    output << json_data.dump(4);  // Use pretty print with indentation of 4 spaces
    output.close();
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    vxe::log_error("Error: ", e.what());
  }
}

VORTEX_API void vxe::update_environment_project(const std::string &oldname) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  std::string path = vxe::get_home_directory() + "/.vx/data/";
  std::string json_file = path + "/projects.json";

  // Verify if the project is valid
  try {
    // Load JSON data from the project configuration file
    auto json_data = vxe::dump_json(json_file);

    // Check if a project with the old name exists
    bool projectExists = false;
    for (auto &project : json_data["projects"]) {
      if (project["name"].get<std::string>() == oldname) {
        // Project with the old name exists, update its information
        project["name"] = ctx->name;
        project["version"] = ctx->version;
        project["description"] = ctx->description;
        project["path"] = ctx->projectPath;
        project["logoPath"] = ctx->logoPath;
        projectExists = true;
        break;
      }
    }

    // If the project doesn't exist, create a new JSON object and add it to the
    // list
    if (!projectExists) {
      json_data["projects"].push_back(
          { { "name", ctx->name },
            { "version", ctx->version },
            { "description", ctx->description },
            { "path", ctx->projectPath },
            { "logoPath", ctx->logoPath } });
    }

    // Write the updated JSON data back to the file
    std::ofstream output(json_file);
    output << json_data.dump(4);  // Use pretty print with indentation of 4 spaces
    output.close();
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    vxe::log_error("Error: ", e.what());
  }
}
