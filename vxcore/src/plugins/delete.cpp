#include "../../include/plugins/delete.h"
#include "../../include/plugins/load.h"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Delete a plugin of the system
 *
 * @param name Name of the system plugin.
 * @param version Version of the system plugin.
 */
VORTEX_API void vxe::DeleteProjectPlugin(const std::string &name,
                                         const std::string &version) {
  if (name.empty() || version.empty()) {
    vxe::LogError("Core",
                  "DeleteProjectPlugin: name and version must not be empty.");
    return;
  }

  auto containsDangerousChars = [](const std::string &s) {
    const std::string forbidden = "\\/<>|;$`&*?~{}[]()\"'!\n\r\t";
    return s.find_first_of(forbidden) != std::string::npos;
  };

  if (containsDangerousChars(name) || containsDangerousChars(version)) {
    vxe::LogError(
        "Core",
        "DeleteProjectPlugin: name or version contains forbidden characters.");
    return;
  }

  VxContext &ctx = *CVortexMaker;

  if (ctx.projectPath.empty()) {
    vxe::LogError("Core",
                  "DeleteProjectPlugin: projectPath is not set in context.");
    return;
  }

  std::error_code ec;

  const std::filesystem::path allowedBase =
      std::filesystem::canonical(ctx.projectPath / ".vx" / "plugins", ec);

  if (ec) {
    vxe::LogError("Core",
                  "DeleteProjectPlugin: cannot resolve allowed base path: " +
                      ec.message());
    return;
  }

  bool found = false;

  for (auto &plugin : ctx.IO.ep) {
    if (plugin->m_name != name || plugin->m_version != version)
      continue;

    found = true;

    const std::filesystem::path pluginPath = plugin->m_path;

    if (!pluginPath.is_absolute()) {
      vxe::LogError("Core",
                    "DeleteProjectPlugin: plugin path is not absolute: " +
                        pluginPath.string());
      return;
    }

    if (!std::filesystem::exists(pluginPath, ec) || ec) {
      vxe::LogError("Core",
                    "DeleteProjectPlugin: plugin path does not exist: " +
                        pluginPath.string());
      return;
    }
    if (!std::filesystem::is_directory(pluginPath, ec) || ec) {
      vxe::LogError("Core",
                    "DeleteProjectPlugin: plugin path is not a directory: " +
                        pluginPath.string());
      return;
    }

    const std::filesystem::path canonical =
        std::filesystem::canonical(pluginPath, ec);
    if (ec) {
      vxe::LogError("Core",
                    "DeleteProjectPlugin: cannot canonicalize plugin path: " +
                        pluginPath.string());
      return;
    }

    auto [a, b] = std::mismatch(allowedBase.begin(), allowedBase.end(),
                                canonical.begin());
    if (a != allowedBase.end() || canonical == allowedBase) {
      vxe::LogError("Core", "DeleteProjectPlugin: refusing to delete "
                            "path outside of .vx/plugins/: " +
                                canonical.string());
      return;
    }

    const std::filesystem::path relative =
        std::filesystem::relative(canonical, allowedBase, ec);
    if (ec || relative.empty() || relative.begin() == relative.end()) {
      vxe::LogError("Core",
                    "DeleteProjectPlugin: cannot compute relative path for: " +
                        canonical.string());
      return;
    }

    if (std::distance(relative.begin(), relative.end()) != 1) {
      vxe::LogError("Core", "DeleteProjectPlugin: plugin path is not a "
                            "direct child of .vx/plugins/: " +
                                canonical.string());
      return;
    }

    const std::uintmax_t removed = std::filesystem::remove_all(canonical, ec);
    if (ec || removed == static_cast<std::uintmax_t>(-1)) {
      vxe::LogError("Core", "DeleteProjectPlugin: deletion failed for \"" +
                                name + "\" v" + version + " — " + ec.message());
      return;
    }

    vxe::LogInfo("Core", "System plugin \"" + name + "\" v" + version +
                             " deleted (" + std::to_string(removed) +
                             " entries removed).");

    return;
  }

  if (!found) {
    vxe::LogError("Core", "DeleteProjectPlugin: no plugin named \"" + name +
                              "\" v" + version + " found.");
  }
}