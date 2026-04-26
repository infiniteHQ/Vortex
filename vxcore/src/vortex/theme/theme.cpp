#include "../../../include/vortex/logger/logger.hpp"
#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::theme_rebuilded() {
  VxContext &ctx = *CVortexMaker;
  ctx.IO.theme_changed = false;
}

VORTEX_API void vxe::rebuild_theme() {
  VxContext &ctx = *CVortexMaker;
  ctx.IO.theme_changed = true;
}

VORTEX_API bool vxe::is_theme_needs_rebuild() {
  VxContext &ctx = *CVortexMaker;
  return ctx.IO.theme_changed;
}

VORTEX_API void vxe::refresh_project_themes() {
  VxContext &ctx = *CVortexMaker;

  std::string home = ctx.projectPath.string();
  std::string themes_path = home + "/.vx/configs/themes/data";
  std::string config_path = home + "/.vx/configs/themes";
  std::string json_file = config_path + "/themes.json";

  vxe::create_folder_if_not_exists(themes_path);
  vxe::create_folder_if_not_exists(config_path);

  nlohmann::json defaultData = { { "used_theme", "dark" }, { "override_themes", nlohmann::json::array() } };
  vxe::create_file_if_not_exists(json_file, defaultData);

  ctx.IO.themes.clear();

  for (const auto &entry : fs::directory_iterator(themes_path)) {
    if (entry.path().extension() == ".json") {
      try {
        std::ifstream file(entry.path());
        nlohmann::json j;
        file >> j;

        if (j.contains("label") && j.contains("name") && j.contains("theme")) {
          auto themeObj = std::make_shared<Theme>();
          themeObj->label = j.value("label", "");
          themeObj->name = j.value("name", "");
          themeObj->description = j.value("description", "");
          themeObj->authors = j.value("authors", "");

          for (auto &[key, value] : j["theme"].items()) {
            themeObj->theme[key] = value;
          }

          ctx.IO.themes.push_back(themeObj);
        }
      } catch (const std::exception &e) {
        vxe::log_error("Core", "Failed to parse theme file: " + entry.path().string());
        vxe::log_error("Core", e.what());
      }
    }
  }

  try {
    std::ifstream configFile(json_file);
    nlohmann::json configJson;
    configFile >> configJson;

    ctx.IO.used_theme = configJson.value("used_theme", "dark");
    ctx.IO.override_themes = configJson.value("override_themes", std::vector<std::string>());
  } catch (const std::exception &e) {
    vxe::log_error("Core", "Failed to load theme config: " + std::string(e.what()));
  }
}

VORTEX_API void update_project_theme(const std::shared_ptr<Theme> &theme, const std::string &title) {
  std::string themes_path = vxe::get_home_directory() + "/.vx/configs/themes/data";
  vxe::create_folder_if_not_exists(themes_path);

  nlohmann::json j;
  j["label"] = theme->label;
  j["name"] = theme->name;
  j["description"] = theme->description;
  j["authors"] = theme->authors;

  nlohmann::json themeValues;

  for (const auto &[key, value] : theme->theme) {
    themeValues[key] = value;
  }

  j["theme"] = themeValues;

  std::string filepath = themes_path + "/" + theme->label + ".json";

  try {
    std::ofstream out(filepath);
    out << std::setw(4) << j << std::endl;
    vxe::log_info("Core", "Theme '" + theme->label + "' updated.");
  } catch (const std::exception &e) {
    vxe::log_error("Core", "Failed to update theme file: " + filepath);
    vxe::log_error("Core", e.what());
  }
}

VORTEX_API std::shared_ptr<Theme> vxe::get_theme(const std::string &label) {
  VxContext &ctx = *CVortexMaker;
  const auto &themes = ctx.IO.themes;

  for (const auto &theme : themes) {
    if (theme) {
      if (theme->label == label) {
        return theme;
      }
    }
  }
  return nullptr;
}
VORTEX_API std::shared_ptr<Theme> vxe::get_selected_theme() {
  VxContext &ctx = *CVortexMaker;
  const std::string &used = ctx.IO.used_theme;
  const auto &themes = ctx.IO.themes;

  for (const auto &theme : themes) {
    if (theme) {
      if (theme->label == used) {
        return theme;
      }
    }
  }

  vxe::log_error("Core", "No theme matched used_theme: '" + used + "'");
  return nullptr;
}

VORTEX_API void vxe::create_new_theme(const std::shared_ptr<Theme> &base_theme, const std::string &title) {
  if (!base_theme) {
    vxe::log_error("Theme", "Base theme is null. Cannot create new theme.");
    return;
  }

  std::string themes_path = vxe::get_current_context()->projectPath.string() + "/.vx/configs/themes/data";
  vxe::create_folder_if_not_exists(themes_path);

  std::string base_filename = title;
  std::transform(base_filename.begin(), base_filename.end(), base_filename.begin(), [](unsigned char c) {
    return std::isspace(c) ? '_' : std::tolower(c);
  });

  std::string final_filename = base_filename + ".json";
  std::string final_path = themes_path + "/" + final_filename;
  int counter = 1;
  while (fs::exists(final_path)) {
    final_filename = base_filename + "_" + std::to_string(counter) + ".json";
    final_path = themes_path + "/" + final_filename;
    ++counter;
  }

  nlohmann::json theme_json;
  theme_json["label"] = base_filename;
  theme_json["name"] = title;
  theme_json["description"] = base_theme->description;
  theme_json["authors"] = base_theme->authors;

  nlohmann::json theme_data;
  for (const auto &[key, value] : base_theme->theme) {
    theme_data[key] = value;
  }

  theme_json["theme"] = theme_data;

  try {
    std::ofstream file(final_path);
    file << std::setw(4) << theme_json << std::endl;
    vxe::log_info("Theme", "New theme created: " + final_filename);
  } catch (const std::exception &e) {
    vxe::log_error("Theme", "Failed to write new theme file: " + std::string(e.what()));
  }
}

VORTEX_API void vxe::verify_and_populate_themes() {
  std::string themes_path = vxe::get_current_context()->projectPath.string() + "/.vx/configs/themes/data";

  vxe::create_folder_if_not_exists(themes_path);

  const std::vector<std::string> required_themes = { "dark.json", "clear.json" };

  for (const auto &theme_file : required_themes) {
    std::string path = themes_path + "/" + theme_file;
    bool needs_creation = false;

    if (!fs::exists(path)) {
      vxe::log_info("Theme", theme_file + " is missing. Recreating.");
      needs_creation = true;
    } else {
      try {
        std::ifstream in(path);
        nlohmann::json theme_json;
        in >> theme_json;

        if (!theme_json.contains("label") || !theme_json.contains("name") || !theme_json.contains("description") ||
            !theme_json.contains("authors") || !theme_json.contains("theme")) {
          vxe::log_error("Theme", theme_file + " is invalid. Recreating.");
          needs_creation = true;
        }
      } catch (...) {
        vxe::log_error("Theme", theme_file + " is corrupted. Recreating.");
        needs_creation = true;
      }
    }

    if (needs_creation && theme_file == "dark.json") {
      std::shared_ptr<Theme> dark_theme = std::make_shared<Theme>();
      dark_theme->description = "Default dark theme";
      dark_theme->authors = { "Vortex DevTeam" };

      // clang-format off
      std::map<std::string, std::string> properties = {
          {"color_header", "#2F2F2FFF"},
          {"color_header_hovered", "#2F2F2FFF"},
          {"color_header_active", "#2F2F2FFF"},
          {"color", "#383838C8"},
          {"color_hovered", "#464646FF"},
          {"color_active", "#38383896"},
          {"color_framebg", "#0F0F0FFF"},
          {"color_framebg_hovered", "#0F0F0FFF"},
          {"color_framebg_active", "#0F0F0FFF"},
          {"color_tab", "#151515FF"},
          {"color_tab_hovered", "#24242464"},
          {"color_tab_active", "#242424FF"},
          {"color_tab_unfocused", "#151515FF"},
          {"color_tab_unfocused_active", "#24242464"},
          {"color_text", "#898989FF"},
          {"color_titlebg", "#151515FF"},
          {"color_titlebg_active", "#151515FF"},
          {"color_titlebg_collapsed", "#262626FF"},
          {"color_resizegrip", "#E8E8E840"},
          {"color_resizegrip_hovered", "#CFCFCFAA"},
          {"color_resizegrip_active", "#757575F2"},
          {"color_scrollbar_bg", "#05050587"},
          {"color_scrollbar_grab", "#4F4F4FFF"},
          {"color_scrollbar_grab_hovered", "#696969FF"},
          {"color_scrollbar_grab_active", "#828282FF"},
          {"color_checkmark", "#C8C8C8FF"},
          {"color_slidergrab", "#828282B3"},
          {"color_slidergrab_active", "#A8A8A8FF"},
          {"color_separator", "#151515FF"},
          {"color_separator_active", "#27B9F2FF"},
          {"color_separator_hovered", "#151515FF"},
          {"color_window_bg", "#151515FF"},
          {"color_child_bg", "#242424FF"},
          {"color_popup_bg", "#383838FF"},
          {"color_border", "#242424FF"},
          {"color_table_header_bg", "#2F2F2FFF"},
          {"color_table_border_light", "#1A1A1AFF"},
          {"color_menubar_bg", "#242424FF"},

          // Buttons
          {"button_color_border", "#454545FF"},
          {"button_color_border_hovered", "#555555FF"},
          {"button_color_border_clicked", "#444444FF"},
          {"button_color_border_pressed", "#757575FF"},
          {"button_color_bg", "#242424FF"},
          {"button_color_bg_hovered", "#343434FF"},
          {"button_color_bg_pressed", "#444444FF"},
          {"button_color_bg_clicked", "#444444FF"},
          {"button_color_text", "#BCBCBCFF"},
          {"button_color_text_hovered", "#FFFFFFFF"},
          {"button_color_text_pressed", "#FFFFFFFF"},
          {"button_color_underline", "#242424FF"},
          {"button_color_underline_hovered", "#343434FF"},
          {"button_color_underline_pressed", "#444444FF"},
          {"button_size_x", "0"},
          {"button_size_y", "0"},
          {"button_padding_x", "6"},
          {"button_padding_y", "6"},
          {"button_scale", "0"},

          // Checkbox
          {"checkbox_color_border", "#454545B2"},
          {"checkbox_color_border_hovered", "#454545B2"},
          {"checkbox_color_border_clicked", "#454545B2"},
          {"checkbox_color_border_pressed", "#454545B2"},
          {"checkbox_color_bg", "#242424FF"},
          {"checkbox_color_bg_hovered", "#343434FF"},
          {"checkbox_color_bg_pressed", "#444444FF"},
          {"checkbox_color_bg_clicked", "#444444FF"},
          {"checkbox_size_x", "6"},
          {"checkbox_size_y", "6"},

          // Combo
          {"combo_color_border", "#454545B2"},
          {"combo_color_border_hovered", "#454545B2"},
          {"combo_color_border_clicked", "#454545B2"},
          {"combo_color_border_pressed", "#454545B2"},
          {"combo_color_bg", "#242424FF"},
          {"combo_color_bg_hovered", "#343434FF"},
          {"combo_color_bg_pressed", "#444444FF"},
          {"combo_color_bg_clicked", "#444444FF"},
          {"combo_size_x", "6"},
          {"combo_size_y", "6"},

          // Header
          {"header_color_border", "#454545B2"},
          {"header_color_border_hovered", "#454545B2"},
          {"header_color_border_clicked", "#454545B2"},
          {"header_color_border_pressed", "#454545B2"},
          {"header_color_bg", "#242424FF"},
          {"header_color_bg_hovered", "#343434FF"},
          {"header_color_bg_pressed", "#444444FF"},
          {"header_color_bg_clicked", "#444444FF"},
          {"header_size_x", "6"},
          {"header_size_y", "6"},
          {"header_button_color_border", "#454545FF"},
          {"header_button_color_border_hovered", "#555555FF"},
          {"header_button_color_border_clicked", "#444444FF"},
          {"header_button_color_border_pressed", "#757575FF"},
          {"header_button_color_bg", "#242424FF"},
          {"header_button_color_bg_hovered", "#343434FF"},
          {"header_button_color_bg_pressed", "#444444FF"},
          {"header_button_color_bg_clicked", "#444444FF"},
          {"header_button_color_text", "#BCBCBCFF"},
          {"header_button_color_text_hovered", "#FFFFFFFF"},
          {"header_button_color_text_pressed", "#FFFFFFFF"},

          // Image
          {"image_color_border", "#454545B2"},
          {"image_color_border_hovered", "#454545B2"},
          {"image_color_border_clicked", "#454545B2"},
          {"image_color_border_pressed", "#454545B2"},
          {"image_color_bg", "#242424FF"},
          {"image_color_bg_hovered", "#343434FF"},
          {"image_color_bg_pressed", "#444444FF"},
          {"image_color_bg_clicked", "#444444FF"},
          {"image_size_x", "6"},
          {"image_size_y", "6"},

          // Modal
          {"modal_padding_x", "6"},
          {"modal_padding_y", "6"},

          // Notification
          {"notification_color_border", "#454545FF"},
          {"notification_color_border_hovered", "#555555FF"},
          {"notification_color_bg", "#242424FF"},
          {"notification_color_bg_hovered", "#343434FF"},

          // Separator
          {"separator_color", "#424242"},
          {"separator_color_text", "#454545B2"},

          // Keyval
          {"keyval_color_text", "#454545B2"},

          // Table
          {"table_color_border", "#232323"},
          {"table_cell_padding_x_header", "6.0"},
          {"table_cell_padding_y_header", "6.0"},
          {"table_cell_padding_x_row", "6.0"},
          {"table_cell_padding_y_row", "6.0"},

          // Text Area
          {"text_area_color_text", "#FFFFFFFF"},

          // Text
          {"text_color_text", "#454545B2"},

          // Titles
          {"title_color_text", "#FFFFFFFF"},

          // Tooltip
          {"tooltip_color_border", "#454545B2"},
          {"tooltip_color_border_hovered", "#454545B2"},
          {"tooltip_color_border_clicked", "#454545B2"},
          {"tooltip_color_bg", "#242424FF"},
          {"tooltip_color_bg_hovered", "#343434FF"},
          {"tooltip_color_bg_clicked", "#444444FF"},

          // Blocks
          {"block_color", "#252525"},
          {"block_color_hovered", "#454545"},
          {"block_color_pressed", "#555555"},
          {"block_border_color", "#353535"},
          {"block_border_color_hovered", "#353535"},
          {"block_border_color_pressed", "#555555"},
          {"block_border_radius", "0.0"},
          {"block_border_size", "1.0"},
      };
      // clang-format on

      for (const auto &[k, v] : properties) {
        dark_theme->theme[k] = v;
      }

      create_new_theme(dark_theme, "Dark");
    } else if (needs_creation && theme_file == "clear.json") {
      std::shared_ptr<Theme> light_theme = std::make_shared<Theme>();
      light_theme->description = "Default light theme";
      light_theme->authors = { "Vortex DevTeam" };

      std::map<std::string, std::string> properties = { { "color_header", "#EAEAEA" },
                                                        { "color_header_hovered", "#DCDCDC" },
                                                        { "color_header_active", "#D0D0D0" },
                                                        { "color", "#F5F5F5" },
                                                        { "color_hovered", "#EBEBEB" },
                                                        { "color_active", "#DADADA" },
                                                        { "color_framebg", "#FFFFFF" },
                                                        { "color_framebg_hovered", "#F0F0F0" },
                                                        { "color_framebg_active", "#E0E0E0" },
                                                        { "color_text", "#252525FF" },
                                                        { "color_tab", "#F2F2F2" },
                                                        { "color_tab_hovered", "#E6E6E6" },
                                                        { "color_tab_active", "#DDDDDD" },
                                                        { "color_tab_unfocused", "#F2F2F2" },
                                                        { "color_tab_unfocused_active", "#E6E6E6" },
                                                        { "color_titlebg", "#F0F0F0" },
                                                        { "color_titlebg_active", "#E0E0E0" },
                                                        { "color_titlebg_collapsed", "#FAFAFA" },
                                                        { "color_resizegrip", "#AAAAAA40" },
                                                        { "color_resizegrip_hovered", "#888888AA" },
                                                        { "color_resizegrip_active", "#3D7EFF" },
                                                        { "color_scrollbar_bg", "#EEEEEE" },
                                                        { "color_scrollbar_grab", "#C2C2C2" },
                                                        { "color_scrollbar_grab_hovered", "#A8A8A8" },
                                                        { "color_scrollbar_grab_active", "#8F8F8F" },
                                                        { "color_checkmark", "#3D7EFF" },
                                                        { "color_slidergrab", "#BBBBBB" },
                                                        { "color_slidergrab_active", "#3D7EFF" },
                                                        { "color_separator", "#DDDDDD" },
                                                        { "color_separator_active", "#3D7EFF" },
                                                        { "color_separator_hovered", "#CCCCCC" },
                                                        { "color_window_bg", "#FFFFFF" },
                                                        { "color_child_bg", "#F9F9F9" },
                                                        { "color_popup_bg", "#FFFFFF" },
                                                        { "color_border", "#DDDDDD" },
                                                        { "color_table_header_bg", "#EDEDED" },
                                                        { "color_table_border_light", "#F3F3F3" },
                                                        { "color_menubar_bg", "#FAFAFA" },

                                                        // Buttons
                                                        { "button_color_border", "#CCCCCC" },
                                                        { "button_color_border_hovered", "#BBBBBB" },
                                                        { "button_color_border_clicked", "#AAAAAA" },
                                                        { "button_color_border_pressed", "#3D7EFF" },
                                                        { "button_color_bg", "#F0F0F0" },
                                                        { "button_color_bg_hovered", "#E6E6E6" },
                                                        { "button_color_bg_pressed", "#DADADA" },
                                                        { "button_color_bg_clicked", "#DADADA" },
                                                        { "button_color_text", "#333333" },
                                                        { "button_color_text_hovered", "#000000" },
                                                        { "button_color_text_pressed", "#000000" },
                                                        { "button_color_underline", "#CCCCCC" },
                                                        { "button_color_underline_hovered", "#AAAAAA" },
                                                        { "button_color_underline_pressed", "#3D7EFF" },
                                                        { "button_size_x", "0" },
                                                        { "button_size_y", "0" },
                                                        { "button_padding_x", "6" },
                                                        { "button_padding_y", "6" },
                                                        { "button_scale", "0" },

                                                        // Checkbox
                                                        { "checkbox_color_border", "#CCCCCC" },
                                                        { "checkbox_color_border_hovered", "#AAAAAA" },
                                                        { "checkbox_color_border_clicked", "#888888" },
                                                        { "checkbox_color_border_pressed", "#3D7EFF" },
                                                        { "checkbox_color_bg", "#FFFFFF" },
                                                        { "checkbox_color_bg_hovered", "#F5F5F5" },
                                                        { "checkbox_color_bg_pressed", "#E5E5E5" },
                                                        { "checkbox_color_bg_clicked", "#E5E5E5" },
                                                        { "checkbox_size_x", "6" },
                                                        { "checkbox_size_y", "6" },

                                                        // Tooltip
                                                        { "tooltip_color_border", "#CCCCCC" },
                                                        { "tooltip_color_border_hovered", "#AAAAAA" },
                                                        { "tooltip_color_border_clicked", "#888888" },
                                                        { "tooltip_color_bg", "#FAFAFA" },
                                                        { "tooltip_color_bg_hovered", "#F0F0F0" },
                                                        { "tooltip_color_bg_clicked", "#E0E0E0" },

                                                        // Text Area
                                                        { "text_area_color_text", "#222222" },

                                                        // Text
                                                        { "text_color_text", "#222222" },

                                                        // Titles
                                                        { "title_color_text", "#000000" },

                                                        // Notification
                                                        { "notification_color_border", "#AAAAAA" },
                                                        { "notification_color_border_hovered", "#3D7EFF" },
                                                        { "notification_color_bg", "#FFFFFF" },
                                                        { "notification_color_bg_hovered", "#F7F7F7" },

                                                        // Blocks
                                                        { "block_color", "#F5F5F5" },
                                                        { "block_color_hovered", "#E5E5E5" },
                                                        { "block_color_pressed", "#D5D5D5" },
                                                        { "block_border_color", "#CCCCCC" },
                                                        { "block_border_color_hovered", "#BBBBBB" },
                                                        { "block_border_color_pressed", "#3D7EFF" },
                                                        { "block_border_radius", "0.0" },
                                                        { "block_border_size", "1.0" } };

      for (const auto &[k, v] : properties) {
        light_theme->theme[k] = v;
      }

      create_new_theme(light_theme, "Clear");
    }
  }
}

VORTEX_API void vxe::update_project_themes_config() {
  VxContext &ctx = *CVortexMaker;
  std::string config_path = vxe::get_home_directory() + "/.vx/configs/themes";
  std::string json_file = config_path + "/themes.json";

  nlohmann::json configJson;
  configJson["used_theme"] = ctx.IO.used_theme;
  configJson["override_themes"] = ctx.IO.override_themes;

  try {
    std::ofstream out(json_file);
    out << std::setw(4) << configJson << std::endl;
    vxe::log_info("Core", "Theme configuration updated.");
  } catch (const std::exception &e) {
    vxe::log_error("Core", "Failed to update theme config file: " + std::string(e.what()));
  }
}
