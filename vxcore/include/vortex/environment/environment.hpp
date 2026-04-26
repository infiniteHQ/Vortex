#pragma once
#include <vortex.h>

#ifndef VORTEX_ENVIRONMENT_HPP
#define VORTEX_ENVIRONMENT_HPP

namespace vxe {
  VORTEX_API bool is_linux();
  VORTEX_API bool is_not_linux();
  VORTEX_API bool is_windows();
  VORTEX_API bool is_not_windows();
  VORTEX_API bool is_macos();
  VORTEX_API bool is_not_macos();
  VORTEX_API void detect_platform();
  VORTEX_API void detect_arch();
  VORTEX_API void refresh_environment_projects();
  VORTEX_API void init_environment();
  VORTEX_API void initialize_platform_vendor();
  VORTEX_API void refresh_environment_vortex_versions_pools();
  VORTEX_API void update_environment_project();
  VORTEX_API void update_environment_project(const std::string &oldname);
  VORTEX_API void update_environment_project(
      const std::string &name,
      const std::string &author,
      const std::string &version,
      const std::string &compatibleWith,
      const std::string &description,
      const std::string &path,
      const std::string &logo_path,
      const std::string &template_name);

}  // namespace vxe

#endif  // VORTEX_ENVIRONMENT_HPP