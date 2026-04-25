#include "../../include/modules/delete.h"
#include "../../include/modules/load.h"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Delete a module of the system
 *
 * @param name Name of the system module.
 * @param version Version of the system module.
 */
VORTEX_API void vxe::DeleteProjectModule(const std::string &name,
                                         const std::string &version) {
  if (name.empty() || version.empty()) {
    vxe::LogError("Core",
                  "DeleteProjectModule: name and version must not be empty.");
    return;
  }

  auto containsDangerousChars = [](const std::string &s) {
    const std::string forbidden = "\\/<>|;$`&*?~{}[]()\"'!\n\r\t";
    return s.find_first_of(forbidden) != std::string::npos;
  };

  if (containsDangerousChars(name) || containsDangerousChars(version)) {
    vxe::LogError(
        "Core",
        "DeleteProjectModule: name or version contains forbidden characters.");
    return;
  }

  VxContext &ctx = *CVortexMaker;

  if (ctx.projectPath.empty()) {
    vxe::LogError("Core",
                  "DeleteProjectModule: projectPath is not set in context.");
    return;
  }

  std::error_code ec;

  const std::filesystem::path allowedBase =
      std::filesystem::canonical(ctx.projectPath / ".vx" / "modules", ec);

  if (ec) {
    vxe::LogError("Core",
                  "DeleteProjectModule: cannot resolve allowed base path: " +
                      ec.message());
    return;
  }

  bool found = false;

  for (auto &module : ctx.IO.em) {
    if (module->m_name != name || module->m_version != version)
      continue;

    found = true;

    const std::filesystem::path modulePath = module->m_path;

    if (!modulePath.is_absolute()) {
      vxe::LogError("Core",
                    "DeleteProjectModule: module path is not absolute: " +
                        modulePath.string());
      return;
    }

    if (!std::filesystem::exists(modulePath, ec) || ec) {
      vxe::LogError("Core",
                    "DeleteProjectModule: module path does not exist: " +
                        modulePath.string());
      return;
    }
    if (!std::filesystem::is_directory(modulePath, ec) || ec) {
      vxe::LogError("Core",
                    "DeleteProjectModule: module path is not a directory: " +
                        modulePath.string());
      return;
    }

    const std::filesystem::path canonical =
        std::filesystem::canonical(modulePath, ec);
    if (ec) {
      vxe::LogError("Core",
                    "DeleteProjectModule: cannot canonicalize module path: " +
                        modulePath.string());
      return;
    }

    auto [a, b] = std::mismatch(allowedBase.begin(), allowedBase.end(),
                                canonical.begin());
    if (a != allowedBase.end() || canonical == allowedBase) {
      vxe::LogError("Core", "DeleteProjectModule: refusing to delete "
                            "path outside of .vx/modules/: " +
                                canonical.string());
      return;
    }

    const std::filesystem::path relative =
        std::filesystem::relative(canonical, allowedBase, ec);
    if (ec || relative.empty() || relative.begin() == relative.end()) {
      vxe::LogError("Core",
                    "DeleteProjectModule: cannot compute relative path for: " +
                        canonical.string());
      return;
    }

    if (std::distance(relative.begin(), relative.end()) != 1) {
      vxe::LogError("Core", "DeleteProjectModule: module path is not a "
                            "direct child of .vx/modules/: " +
                                canonical.string());
      return;
    }

    const std::uintmax_t removed = std::filesystem::remove_all(canonical, ec);
    if (ec || removed == static_cast<std::uintmax_t>(-1)) {
      vxe::LogError("Core", "DeleteProjectModule: deletion failed for \"" +
                                name + "\" v" + version + " — " + ec.message());
      return;
    }

    vxe::LogInfo("Core", "System module \"" + name + "\" v" + version +
                             " deleted (" + std::to_string(removed) +
                             " entries removed).");

    return;
  }

  if (!found) {
    vxe::LogError("Core", "DeleteProjectModule: no module named \"" + name +
                              "\" v" + version + " found.");
  }
}