//
//  delete.cpp
//  Sources for plugins deleting functions
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../include/plugins/delete.h"

#include "../../include/plugins/load.h"
#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

VORTEX_API void vxe::delete_project_plugin(const std::string &name, const std::string &version) {
  if (name.empty() || version.empty()) {
    vxe::log_error("Core", "delete_project_plugin: name and version must not be empty.");
    return;
  }

  auto containsDangerousChars = [](const std::string &s) {
    const std::string forbidden = "\\/<>|;$`&*?~{}[]()\"'!\n\r\t";
    return s.find_first_of(forbidden) != std::string::npos;
  };

  if (containsDangerousChars(name) || containsDangerousChars(version)) {
    vxe::log_error("Core", "delete_project_plugin: name or version contains forbidden characters.");
    return;
  }

  auto ctx = vxe::get_current_context();

  if (ctx->projectPath.empty()) {
    vxe::log_error("Core", "delete_project_plugin: projectPath is not set in context.");
    return;
  }

  std::error_code ec;

  const std::filesystem::path allowedBase = std::filesystem::canonical(ctx->projectPath / ".vx" / "plugins", ec);

  if (ec) {
    vxe::log_error("Core", "delete_project_plugin: cannot resolve allowed base path: " + ec.message());
    return;
  }

  bool found = false;

  for (auto &plugin : ctx->IO.ep) {
    if (plugin->m_name != name || plugin->m_version != version)
      continue;

    found = true;

    const std::filesystem::path pluginPath = plugin->m_path;

    if (!pluginPath.is_absolute()) {
      vxe::log_error("Core", "delete_project_plugin: plugin path is not absolute: " + pluginPath.string());
      return;
    }

    if (!std::filesystem::exists(pluginPath, ec) || ec) {
      vxe::log_error("Core", "delete_project_plugin: plugin path does not exist: " + pluginPath.string());
      return;
    }
    if (!std::filesystem::is_directory(pluginPath, ec) || ec) {
      vxe::log_error("Core", "delete_project_plugin: plugin path is not a directory: " + pluginPath.string());
      return;
    }

    const std::filesystem::path canonical = std::filesystem::canonical(pluginPath, ec);
    if (ec) {
      vxe::log_error("Core", "delete_project_plugin: cannot canonicalize plugin path: " + pluginPath.string());
      return;
    }

    auto [a, b] = std::mismatch(allowedBase.begin(), allowedBase.end(), canonical.begin());
    if (a != allowedBase.end() || canonical == allowedBase) {
      vxe::log_error(
          "Core",
          "delete_project_plugin: refusing to delete "
          "path outside of .vx/plugins/: " +
              canonical.string());
      return;
    }

    const std::filesystem::path relative = std::filesystem::relative(canonical, allowedBase, ec);
    if (ec || relative.empty() || relative.begin() == relative.end()) {
      vxe::log_error("Core", "delete_project_plugin: cannot compute relative path for: " + canonical.string());
      return;
    }

    if (std::distance(relative.begin(), relative.end()) != 1) {
      vxe::log_error(
          "Core",
          "delete_project_plugin: plugin path is not a "
          "direct child of .vx/plugins/: " +
              canonical.string());
      return;
    }

    const std::uintmax_t removed = std::filesystem::remove_all(canonical, ec);
    if (ec || removed == static_cast<std::uintmax_t>(-1)) {
      vxe::log_error(
          "Core", "delete_project_plugin: deletion failed for \"" + name + "\" v" + version + " — " + ec.message());
      return;
    }

    vxe::log_info(
        "Core",
        "System plugin \"" + name + "\" v" + version + " deleted (" + std::to_string(removed) + " entries removed).");

    return;
  }

  if (!found) {
    vxe::log_error("Core", "delete_project_plugin: no plugin named \"" + name + "\" v" + version + " found.");
  }
}