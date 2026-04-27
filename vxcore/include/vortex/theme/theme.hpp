//
//  theme.hpp
//  Headers for main utilities and some helpers functions
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#ifndef VORTEX_THEME_HPP
#define VORTEX_THEME_HPP

namespace vxe {
  VORTEX_API void theme_rebuilded();
  VORTEX_API void rebuild_theme();
  VORTEX_API bool is_theme_needs_rebuild();
  VORTEX_API void refresh_project_themes();
  VORTEX_API void update_project_theme(const std::shared_ptr<Theme> &theme);
  VORTEX_API void update_project_themes_config();
  VORTEX_API void verify_and_populate_themes();
  VORTEX_API void create_new_theme(const std::shared_ptr<Theme> &base_theme, const std::string &title);
  VORTEX_API std::shared_ptr<Theme> get_theme(const std::string &label);
  VORTEX_API std::shared_ptr<Theme> get_selected_theme();

}  // namespace vxe

#endif  // VORTEX_THEME_HPP