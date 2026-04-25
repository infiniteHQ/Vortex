//
//  content_browser.hpp
//  Header and declarations for the content browser of the editor, with UI
//  window and front-end logic.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "./content_browser_helpers.hpp"

#ifndef VORTEX_EDITOR_CONTENT_BROWSER_HPP
#define VORTEX_EDITOR_CONTENT_BROWSER_HPP

using namespace Cherry;

namespace vxe {

  class ContentBrowser : public std::enable_shared_from_this<ContentBrowser> {
   public:
    ContentBrowser(const std::string &name, const std::string &start_path);

    // window managment
    static std::shared_ptr<ContentBrowser> create(const std::string &name, const std::string &base_path);
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    void setup_render_callback();

    // rendering
    void render();
    void render_side_bar();
    void render_filters_bar();
    void render_content_bar();
    void render_details_bar();
    void render_right_menubar();
    void render_menubar();
    void add_child(const ContentBrowserChild &child);
    void spawn_add_window();
    void spawn_import_window();

    // widgets
    void draw_folder(const std::string &colorHex, float width, float height, ImVec2 pos);
    bool draw_item_card(const ItemCardParams &p);
    bool draw_horizontal_draw_item_card(const ItemCardParams &p);
    void draw_folder_button(const char *id, ImVec2 size, const std::string &path, const std::string &color);
    void draw_hierarchy(std::filesystem::path path, bool isDir, const std::string &label = "");
    void draw_path_bar(const std::string &path);
    void draw_folder_icon(ImVec2 pos, ImVec2 size, ImU32 color);
    bool draw_color_picker_3U32(const char *label, ImU32 *color, ImGuiColorEditFlags flags = 0);
    void draw_highlighted_text_(
        ImDrawList *drawList,
        ImVec2 textPos,
        const char *text,
        const char *search,
        ImU32 highlightColor,
        ImU32 textColor,
        ImU32 highlightTextColor);
    void folder_icon(ImVec2 size, ImU32 color);

    // navigation
    void change_directory(const std::filesystem::path &new_directory);
    void go_back();
    void go_forward();
    void path_changed();
    void select(const std::string &path);
    void refresh_custom_folders();
    void refresh_pools();

    // fs manipulation
    void create_file();
    void create_folder();

    // utils
    void set_default_folder_color(const std::string &hex);
    void set_colored_folder(const std::string &path, const std::string &hex_color);
    void set_delete_path_callback(const std::function<void(const std::string &)> &foo);
    void set_copy_paths_callback(const std::function<void(const std::vector<std::string> &, bool)> &foo);
    void set_cut_paths_callback(const std::function<void(const std::vector<std::string> &, bool)> &foo);
    void set_paste_paths_callback(const std::function<void(const std::string &)> &foo);
    void toggle_extension(const std::string &ext, bool *state);
    void change_show_mode(ContentShowMode mode);
    std::string get_extension(const std::string &path);
    std::string get_content_browser_folder_color(const std::string &path);
    std::string get_file_type_str(FileTypes type);
    std::string format_file_size(size_t size);
    std::string to_lower_case(const std::string &str);
    bool are_strings_similar(const std::string &s1, const std::string &s2, double threshold_);
    bool has_common_letters(const std::string &s1, const std::string &s2);
    bool has_extension(const std::string &ext);
    bool is_only_spaces_or_empty(const char *str);
    bool is_path_favorite(const std::string &path);
    int levenshtein_distance(const std::string &s1, const std::string &s2);
    std::uintmax_t get_directory_size(const std::filesystem::path &directoryPath);
    FileTypes detect_file(const std::string &path);

   private:
    // callbacks
    std::function<void(const std::string &)> delete_path_callback_;
    std::function<void(const std::vector<std::string> &, bool)> copy_paths_callback_;
    std::function<void(const std::vector<std::string> &, bool)> cut_paths_callback_;
    std::function<void(const std::string &)> paste_paths_callback_;

    // directories
    std::filesystem::path base_directory_;
    std::filesystem::path previous_directory_;
    std::filesystem::path current_directory_;

    // history
    std::stack<std::filesystem::path> back_history_;
    std::stack<std::filesystem::path> forward_history_;

    // selection
    std::vector<std::string> selected_;
    std::vector<std::string> copy_selection_;
    std::vector<std::string> cut_selection_;

    // path/color
    std::vector<std::pair<std::string, std::string>> folder_colors_;
    std::string default_folder_color_;
    std::string default_hidden_folder_color_;

    // pools and special items
    std::vector<std::string> favorite_folders_;
    std::vector<std::pair<std::string, std::string>> pools_;
    std::vector<std::shared_ptr<ContenBrowserItem>> item_to_reconize_;
    std::vector<std::filesystem::path> favorites_;

    // filters
    bool show_hidden_ = false;
    bool show_folders_ = true;
    bool show_files_ = true;
    bool show_items_ = true;
    std::vector<std::string> extentions_filters_;

    // items
    std::vector<std::shared_ptr<ItemHandlerInterface>> item_handles_;
    std::vector<std::pair<std::string, std::string>> item_paths_;
    std::vector<std::pair<std::shared_ptr<ItemIdentifierInterface>, std::string>> recognized_modules_items_;

    // ui properties
    float sidebar_width_ = 250.0f;
    float filterbar_width_ = 250.0f;
    float padding = 30.0f;
    float thumbnail_size = 94.0f;
    float threshold_ = 0.4f;
    bool thumbnail_mode_ = true;
    bool list_mode_ = false;
    bool show_folder_pannel_ = true;
    bool show_selection_quantifier_ = false;
    bool show_types_ = false;
    bool show_sizes_ = false;
    bool show_filter_pannel_ = false;
    bool show_thumbnail_visualizer_ = false;

    ContentShowMode show_mode_ = ContentShowMode::Thumbmails;

    // ui states
    std::string path_to_rename_ = "";
    std::string project_search_;
    std::pair<std::string, ImU32> current_editing_folder_;
    bool current_editing_folder_is_favorite_;
    bool search_bar_ = false;
    bool item_context_menu_opened_ = false;
    bool pool_add_mode_ = false;
    bool childs_initialized_ = false;
    bool will_show_filter_pannel_ = false;
    bool previous_filter_pannel_state_ = false;
    bool previous_thumbnail_visualizer_state_ = false;
    char pool_add_path_[512];
    char pool_add_name_[512];

    // hosted
    std::vector<ContentBrowserChild> childs_;
    std::vector<std::shared_ptr<vxe::ContentBrowserAddWindow>> add_windows_;
    int add_windows_counter_ = 0;

    std::shared_ptr<AppWindow> app_window_;
  };
}  // namespace vxe

#endif  // VORTEX_EDITOR_CONTENT_BROWSER_HPP