//
//  content_browser.hpp
//  Headers for all content browser backends and manipulation features.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#include "./modules/interface.hpp"
#include "./plugins/interface.hpp"

#ifndef VORTEX_CONTENT_BROWSER_HPP
#define VORTEX_CONTENT_BROWSER_HPP

#include "content_browser_helpers.hpp"

namespace vxe {
  VORTEX_API std::string create_file(const std::string &path);
  VORTEX_API std::string create_folder(const std::string &path);
  VORTEX_API void move_all_content();
  VORTEX_API void copy_all_content();

  VORTEX_API bool content_browser_folder_is_fav(const std::string &path);
  VORTEX_API bool get_content_browser_folder_color(const std::string &path, ImU32 *color);

  VORTEX_API void copy(std::vector<std::string> selection, bool in_addition);
  VORTEX_API void cut(std::vector<std::string> selection, bool in_addition);
  VORTEX_API void clear_copy_selection();
  VORTEX_API void clear_cut_selection();
  VORTEX_API void paste_all_selections(const std::string &target_path);

  VORTEX_API void delete_folder(const std::string &target_path);
  VORTEX_API void delete_file(const std::string &target_path);
  VORTEX_API void delete_path(const std::string &target_path);

  VORTEX_API void rename_folder(const std::string &target_path, const std::string &new_name);
  VORTEX_API void rename_file(const std::string &target_path, const std::string &new_name);

  VORTEX_API void fetch_custom_folders();
  VORTEX_API void fetch_pools();

  VORTEX_API void
  publish_content_browser_custom_folder(const std::string &path, const std::string &hex_color, const bool &isFav);
  VORTEX_API void publish_pool(const std::string &absolute_pool_path, const std::string &name);
  VORTEX_API void post_custom_folder_to_json();
  VORTEX_API void post_pools_to_json();

  VORTEX_API void add_general_utility(const std::shared_ptr<ModuleInterfaceUtility> &utility);
  VORTEX_API void add_content_browser_item(const std::shared_ptr<ContenBrowserItem> &item);

  VORTEX_API void submit_rename(const std::string &oldPathStr, const std::string &newName);

  VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>> get_all_item_handlers_for(const std::string &type);

}  // namespace vxe

#endif  // VORTEX_CONTENT_BROWSER_HPP