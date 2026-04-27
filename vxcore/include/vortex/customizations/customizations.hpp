#pragma once
#include <vortex.h>

#ifndef VORTEX_CUSTOMIZATIONS_HPP
#define VORTEX_CUSTOMIZATIONS_HPP

namespace vxe {
  VORTEX_API void push_edit_menu_item(
      const std::string &title,
      const std::function<void()> &action,
      const std::string &logo = "",
      const std::string &section = "");
  VORTEX_API void pop_edit_menu_item(const int &count = 1);
  VORTEX_API void clear_edit_menu_item();
  VORTEX_API void push_custom_menu(const std::string &title, const std::function<void()> &render);
  VORTEX_API void pop_custom_menu(const int &count = 1);
  VORTEX_API void clear_custom_menus();
}  // namespace vxe

#endif  // VORTEX_CUSTOMIZATIONS_HPP