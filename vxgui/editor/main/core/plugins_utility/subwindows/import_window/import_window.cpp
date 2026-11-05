//
//  import_window.cpp
//  Sources for the "import window" of the plugins utility
//
//  Copyright (c) 2026 Infinite
//
//  This work is licensed under the terms of the Apache-2.0 license.
//  For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./import_window.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <sstream>
#include <thread>

#include "../../vxcore/include/plugins/install.hpp"
#include "../../vxcore/include/vortex_internals.h"

namespace vxe {

  namespace {
    struct Palette {
      const char* card;
      const char* border;
      const char* text;
      const char* sub;
      const char* field;
      const char* pillBg;
      const char* pillText;
      const char* neutral;
      const char* neutralHover;
      const char* accent;
      const char* accentHover;
      const char* accentText;
      const char* warn;
      const char* warnHover;
    };

    Palette get_palette() {
      if (CherryApp.GetTheme() == "dark") {
        return { "#232323", "#333333", "#FFFFFF", "#8A8A8A", "#1B1B1B", "#303030", "#BBBBBB",
                 "#2E2E2E", "#3A3A3A", "#B1FF31", "#C3FF53", "#121212", "#EEAA55", "#F5BC74" };
      }
      return { "#FFFFFF", "#D8D8D8", "#232323", "#6B6B6B", "#F1F1F1", "#E6E6E6", "#444444",
               "#E6E6E6", "#D2D2D2", "#3E8E00", "#4FA800", "#FFFFFF", "#C77700", "#D98A12" };
    }

    enum class InstallState { Missing, Same, Different };

    ImVec4 col(const char* hex) {
      return Cherry::HexToRGBA(hex);
    }

    void colored_text(const char* hex, const std::string& text) {
      ImGui::PushStyleColor(ImGuiCol_Text, col(hex));
      ImGui::TextUnformatted(text.c_str());
      ImGui::PopStyleColor();
    }

    void wrapped_text(const char* hex, const std::string& text, float wrap_x) {
      ImGui::PushTextWrapPos(wrap_x);
      colored_text(hex, text);
      ImGui::PopTextWrapPos();
    }

    void centered_text(const char* hex, const std::string& text) {
      float w = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((std::max)(0.0f, (ImGui::GetWindowWidth() - w) * 0.5f));
      colored_text(hex, text);
    }

    bool action_button(const char* label, const char* bg, const char* hover, const char* fg, float width, float height) {
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
      ImGui::PushStyleColor(ImGuiCol_Button, col(bg));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col(hover));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, col(hover));
      ImGui::PushStyleColor(ImGuiCol_Text, col(fg));
      bool clicked = ImGui::Button(label, ImVec2(width, height));
      ImGui::PopStyleColor(4);
      ImGui::PopStyleVar();
      return clicked;
    }

    void pill(const std::string& text, const char* bg, const char* fg) {
      ImVec2 ts = ImGui::CalcTextSize(text.c_str());
      ImVec2 size(ts.x + 16.0f, ts.y + 6.0f);
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImDrawList* dl = ImGui::GetWindowDrawList();
      dl->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), Cherry::HexToImU32(bg), size.y * 0.5f);
      dl->AddText(ImVec2(p.x + 8.0f, p.y + 3.0f), Cherry::HexToImU32(fg), text.c_str());
      ImGui::Dummy(size);
    }

    void rounded_image(ImTextureID texture, ImVec2 size, float rounding) {
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImGui::GetWindowDrawList()->AddImageRounded(
          texture, p, ImVec2(p.x + size.x, p.y + size.y), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), IM_COL32_WHITE, rounding);
      ImGui::Dummy(size);
    }

    struct CardScope {
      ImDrawList* draw_list = nullptr;
      ImVec2 origin;
      float width = 0.0f;
      float pad = 14.0f;
      float wrap_x = 0.0f;
    };

    CardScope begin_card() {
      CardScope c;
      c.draw_list = ImGui::GetWindowDrawList();
      c.width = ImGui::GetContentRegionAvail().x;
      c.wrap_x = ImGui::GetCursorPosX() + c.width - c.pad;
      c.origin = ImGui::GetCursorScreenPos();
      c.draw_list->ChannelsSplit(2);
      c.draw_list->ChannelsSetCurrent(1);
      ImGui::BeginGroup();
      ImGui::Dummy(ImVec2(0.0f, c.pad));
      ImGui::Indent(c.pad);
      return c;
    }

    void end_card(const CardScope& c, const Palette& pal) {
      ImGui::Unindent(c.pad);
      ImGui::Dummy(ImVec2(0.0f, c.pad));
      ImGui::EndGroup();
      ImVec2 max(c.origin.x + c.width, ImGui::GetItemRectMax().y);
      c.draw_list->ChannelsSetCurrent(0);
      c.draw_list->AddRectFilled(c.origin, max, Cherry::HexToImU32(pal.card), 14.0f);
      c.draw_list->AddRect(c.origin, max, Cherry::HexToImU32(pal.border), 14.0f);
      c.draw_list->ChannelsMerge();
    }

    void push_field_style(const Palette& pal) {
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 9.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, col(pal.field));
      ImGui::PushStyleColor(ImGuiCol_Border, col(pal.border));
      ImGui::PushStyleColor(ImGuiCol_Text, col(pal.text));
      ImGui::PushStyleColor(ImGuiCol_PopupBg, col(pal.card));
    }

    void pop_field_style() {
      ImGui::PopStyleColor(4);
      ImGui::PopStyleVar(3);
    }

    void separator(const Palette& pal) {
      ImGui::PushStyleColor(ImGuiCol_Separator, col(pal.border));
      ImGui::Separator();
      ImGui::PopStyleColor();
    }

    bool string_filter(
        const char* id,
        const char* all_label,
        std::string& current,
        const std::set<std::string>& options,
        float width) {
      bool changed = false;
      ImGui::SetNextItemWidth(width);
      if (ImGui::BeginCombo(id, current.empty() ? all_label : current.c_str())) {
        if (ImGui::Selectable(all_label, current.empty())) {
          current.clear();
          changed = true;
        }
        for (const auto& option : options) {
          if (ImGui::Selectable(option.c_str(), option == current)) {
            current = option;
            changed = true;
          }
        }
        ImGui::EndCombo();
      }
      return changed;
    }

    std::size_t ci_find(const std::string& haystack, const std::string& needle) {
      if (needle.empty())
        return std::string::npos;

      auto to_lower = [](const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
        return out;
      };

      std::string h = to_lower(haystack);
      std::string n = to_lower(needle);

      return h.find(n);
    }

    void draw_highlighted_text(const Palette& pal, const std::string& text, std::size_t match_pos, std::size_t match_len) {
      if (text.empty())
        return;

      if (match_pos == std::string::npos || match_len == 0) {
        colored_text(pal.text, text);
        return;
      }

      std::string before = text.substr(0, match_pos);
      std::string match = text.substr(match_pos, match_len);
      std::string after = text.substr(match_pos + match_len);

      if (!before.empty()) {
        colored_text(pal.text, before);
        ImGui::SameLine(0.0f, 0.0f);
      }

      colored_text(pal.accent, match);

      if (!after.empty()) {
        ImGui::SameLine(0.0f, 0.0f);
        colored_text(pal.text, after);
      }
    }

    std::string get_short_path(const std::string& path) {
      std::string normalized = path;
      std::replace(normalized.begin(), normalized.end(), '\\', '/');
      while (!normalized.empty() && normalized.back() == '/')
        normalized.pop_back();

      std::size_t pos = normalized.find("plugins/");
      if (pos == std::string::npos)
        return normalized;

      return normalized.substr(pos);
    }

    std::map<std::string, std::string> get_installed_plugins() {
      std::map<std::string, std::string> installed;
      auto installed_plugins = vxe::get_current_context()->IO.ep;

      for (const auto& p : installed_plugins) {
        if (p)
          installed[p->name()] = p->version();
      }

      return installed;
    }

  }  // namespace

  ImportPluginWindow::ImportPluginWindow(const std::string& name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [this]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(8.0f);
    app_window_->SetInternalPaddingY(8.0f);
  }

  std::shared_ptr<Cherry::AppWindow>& ImportPluginWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<ImportPluginWindow> ImportPluginWindow::create(const std::string& name) {
    auto instance = std::shared_ptr<ImportPluginWindow>(new ImportPluginWindow(name));
    instance->setup_render_callback();
    return instance;
  }

  void ImportPluginWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void ImportPluginWindow::render() {
    static char search_buffer[128] = "";
    static bool restart_after_install = true;
    static std::string feedback_message;
    static std::string author_filter;
    static std::string group_filter;
    static int status_filter = 0;

    static const char* status_labels[] = { "All status", "Not installed", "Installed", "Other version" };

    const Palette pal = get_palette();
    auto ctx = vxe::get_current_context();
    const auto& sys_plugins = ctx->IO.sys_ep;

    std::set<std::string> authors;
    std::set<std::string> groups;
    for (const auto& plugin : sys_plugins) {
      if (!plugin)
        continue;
      if (!plugin->author().empty())
        authors.insert(plugin->author());
      if (!plugin->group().empty())
        groups.insert(plugin->group());
    }
    if (!author_filter.empty() && authors.find(author_filter) == authors.end())
      author_filter.clear();
    if (!group_filter.empty() && groups.find(group_filter) == groups.end())
      group_filter.clear();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 12.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::BeginChild("##import_plugins", ImVec2(0.0f, 0.0f), true);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    Cherry::PushFont("ClashBold");
    colored_text(pal.text, "Import plugins");
    Cherry::PopFont();
    colored_text(pal.sub, "Browse your system's plugins and import them into this project.");
    ImGui::Dummy(ImVec2(0.0f, 8.0f));

    push_field_style(pal);
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##plugin_search", "Search by name...", search_buffer, IM_ARRAYSIZE(search_buffer));

    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    const float gap = ImGui::GetStyle().ItemSpacing.x;
    const float filter_w = (ImGui::GetContentRegionAvail().x - gap * 2.0f) / 3.0f;

    string_filter("##author_filter", "All authors", author_filter, authors, filter_w);
    ImGui::SameLine();
    string_filter("##group_filter", "All groups", group_filter, groups, filter_w);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(filter_w);
    if (ImGui::BeginCombo("##status_filter", status_labels[status_filter])) {
      for (int i = 0; i < IM_ARRAYSIZE(status_labels); i++) {
        if (ImGui::Selectable(status_labels[i], i == status_filter))
          status_filter = i;
      }
      ImGui::EndCombo();
    }
    pop_field_style();

    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, col(pal.accent));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, col(pal.field));
    ImGui::PushStyleColor(ImGuiCol_Text, col(pal.sub));
    ImGui::Checkbox("Restart all plugins after installation", &restart_after_install);
    ImGui::PopStyleColor(3);

    ImGui::Dummy(ImVec2(0.0f, 6.0f));
    separator(pal);
    ImGui::Dummy(ImVec2(0.0f, 8.0f));

    std::string query(search_buffer);
    std::map<std::string, std::string> installed = get_installed_plugins();

    float list_height = feedback_message.empty() ? 0.0f : -ImGui::GetFrameHeightWithSpacing();
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::BeginChild("##plugins_list", ImVec2(0.0f, list_height), true);
    ImGui::PopStyleColor();

    if (sys_plugins.empty()) {
      ImGui::Dummy(ImVec2(0.0f, 28.0f));
      centered_text(pal.sub, "No plugins installed in this system.");
    }

    int shown_count = 0;
    const float button_width = 90.0f;
    const float button_height = 34.0f;

    for (const auto& plugin : sys_plugins) {
      if (!plugin)
        continue;

      std::string name = plugin->name();
      std::string proper_name = plugin->proper_name();
      std::string description = plugin->description();
      std::string version = plugin->version();
      std::string author = plugin->author();
      std::string group = plugin->group();
      std::string plugin_path = plugin->path();
      std::string short_path = get_short_path(plugin_path);

      InstallState state = InstallState::Missing;
      std::string installed_version;
      auto it = installed.find(name);
      if (it != installed.end()) {
        installed_version = it->second;
        state = installed_version == version ? InstallState::Same : InstallState::Different;
      }

      std::size_t proper_name_match = std::string::npos;

      if (!query.empty()) {
        std::size_t name_match = ci_find(name, query);
        proper_name_match = ci_find(proper_name, query);

        if (name_match == std::string::npos && proper_name_match == std::string::npos)
          continue;
      }

      if (!author_filter.empty() && author != author_filter)
        continue;
      if (!group_filter.empty() && group != group_filter)
        continue;
      if (status_filter == 1 && state != InstallState::Missing)
        continue;
      if (status_filter == 2 && state != InstallState::Same)
        continue;
      if (status_filter == 3 && state != InstallState::Different)
        continue;

      shown_count++;

      ImGui::PushID((name + version + plugin_path).c_str());

      CardScope card = begin_card();
      float row_y = ImGui::GetCursorPosY();
      float text_wrap_x = card.wrap_x - button_width - 12.0f;

      ImGui::BeginGroup();

      rounded_image(Cherry::GetTexture(plugin->logo_path()), ImVec2(48.0f, 48.0f), 10.0f);
      ImGui::SameLine(0.0f, 12.0f);

      ImGui::BeginGroup();
      {
        Cherry::PushFont("ClashBold");
        draw_highlighted_text(pal, proper_name.empty() ? name : proper_name, proper_name_match, query.size());
        Cherry::PopFont();

        ImGui::SameLine(0.0f, 8.0f);
        pill(version, pal.pillBg, pal.pillText);

        if (!group.empty()) {
          ImGui::SameLine(0.0f, 8.0f);
          pill(group, pal.accent, pal.accentText);
        }

        if (!description.empty()) {
          ImGui::Dummy(ImVec2(0.0f, 2.0f));
          wrapped_text(pal.sub, description, text_wrap_x);
        }

        if (state == InstallState::Different) {
          ImGui::Dummy(ImVec2(0.0f, 2.0f));
          colored_text(pal.warn, "Installed version: " + installed_version);
        }

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::SetWindowFontScale(0.85f);
        wrapped_text(pal.sub, short_path, text_wrap_x);
        if (!author.empty())
          wrapped_text(pal.sub, "from " + author, text_wrap_x);
        ImGui::SetWindowFontScale(1.0f);
      }
      ImGui::EndGroup();

      ImGui::EndGroup();

      float group_height = ImGui::GetItemRectSize().y;
      ImGui::SetCursorPos(ImVec2(card.wrap_x - button_width, row_y + (group_height - button_height) * 0.5f));

      bool install = false;
      if (state == InstallState::Same) {
        ImGui::BeginDisabled();
        action_button("Installed", pal.neutral, pal.neutralHover, pal.sub, button_width, button_height);
        ImGui::EndDisabled();
      } else if (state == InstallState::Different) {
        install = action_button("Available", pal.warn, pal.warnHover, pal.accentText, button_width, button_height);
      } else {
        install = action_button("Install", pal.accent, pal.accentHover, pal.accentText, button_width, button_height);
      }

      if (install) {
        bool restart = restart_after_install;
        vxe::install_plugin_by_path(name, plugin_path, restart);

        feedback_message = "Plugin \"" + (proper_name.empty() ? name : proper_name) + "\" installed.";
      }

      end_card(card, pal);
      ImGui::PopID();
      ImGui::Dummy(ImVec2(0.0f, 8.0f));
    }

    if (!sys_plugins.empty() && shown_count == 0) {
      ImGui::Dummy(ImVec2(0.0f, 28.0f));
      centered_text(pal.sub, "No plugins match your search and filters.");
    }

    ImGui::EndChild();

    if (!feedback_message.empty()) {
      ImGui::Dummy(ImVec2(0.0f, 4.0f));
      colored_text(pal.accent, feedback_message);
    }

    ImGui::EndChild();
  }

}  // namespace vxe