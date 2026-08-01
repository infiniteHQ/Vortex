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
#include <set>
#include <sstream>
#include <thread>

#include "../../vxcore/include/modules/install.hpp"
#include "../../vxcore/include/vortex_internals.h"

namespace vxe {

  namespace {
    std::size_t ci_find(const std::string &haystack, const std::string &needle) {
      if (needle.empty())
        return std::string::npos;

      auto to_lower = [](const std::string &s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
        return out;
      };

      std::string h = to_lower(haystack);
      std::string n = to_lower(needle);

      return h.find(n);
    }

    void draw_highlighted_text(const std::string &text, std::size_t match_pos, std::size_t match_len) {
      if (text.empty())
        return;

      if (match_pos == std::string::npos || match_len == 0) {
        ImGui::TextUnformatted(text.c_str());
        return;
      }

      std::string before = text.substr(0, match_pos);
      std::string match = text.substr(match_pos, match_len);
      std::string after = text.substr(match_pos + match_len);

      bool need_sameline = false;

      if (!before.empty()) {
        ImGui::TextUnformatted(before.c_str());
        need_sameline = true;
      }

      if (need_sameline)
        ImGui::SameLine(0.0f, 0.0f);
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.78f, 0.25f, 1.0f));
      ImGui::TextUnformatted(match.c_str());
      ImGui::PopStyleColor();

      if (!after.empty()) {
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::TextUnformatted(after.c_str());
      }
    }

    std::string get_folder_name(const std::string &path) {
      std::string trimmed = path;
      while (!trimmed.empty() && (trimmed.back() == '/' || trimmed.back() == '\\'))
        trimmed.pop_back();

      std::size_t pos = trimmed.find_last_of("/\\");
      if (pos == std::string::npos)
        return trimmed;

      return trimmed.substr(pos + 1);
    }

    std::set<std::string> get_installed_module_names() {
      std::set<std::string> installed;
      auto installed_modules = vxe::get_current_context()->IO.em;

      for (const auto &m : installed_modules) {
        if (m)
          installed.insert(m->name());
      }

      return installed;
    }

  }  // namespace

  ImportPluginWindow::ImportPluginWindow(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [this]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(16.0f);
    app_window_->SetInternalPaddingY(16.0f);

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::shared_ptr<Cherry::AppWindow> &ImportPluginWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<ImportPluginWindow> ImportPluginWindow::create(const std::string &name) {
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

    auto ctx = vxe::get_current_context();

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::BeginChild("##import_modules", ImVec2(0.0f, 0.0f), true);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    ImGui::Text("Import modules from system to this project");
    ImGui::TextDisabled("Browse your system's modules and import thems into this project.");
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 4.0f));

    ImGui::PushItemWidth(-1);
    ImGui::InputTextWithHint("##module_search", "Search by name", search_buffer, IM_ARRAYSIZE(search_buffer));
    ImGui::PopItemWidth();

    ImGui::Dummy(ImVec2(0.0f, 4.0f));
    ImGui::Checkbox("Restart all modules after installation", &restart_after_install);

    ImGui::Dummy(ImVec2(0.0f, 6.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 6.0f));

    std::string query(search_buffer);

    std::set<std::string> installed_names = get_installed_module_names();

    ImGui::BeginChild("##modules_list", ImVec2(0.0f, 0.0f), true);

    const auto &sys_modules = ctx->IO.sys_em;

    if (sys_modules.empty()) {
      ImGui::TextDisabled("No modules installed in this system.");
    }

    int shown_count = 0;
    const float button_width = 90.0f;
    const float row_height = 64.0f;

    for (const auto &module : sys_modules) {
      if (!module)
        continue;

      std::string name = module->name();
      std::string proper_name = module->proper_name();
      std::string description = module->description();
      std::string version = module->version();
      std::string author = module->author();
      std::string group = module->group();
      std::string module_path = module->path();
      std::string folder_name = get_folder_name(module_path);

      bool already_installed = installed_names.find(name) != installed_names.end();

      std::size_t name_match = std::string::npos;
      std::size_t proper_name_match = std::string::npos;

      if (!query.empty()) {
        name_match = ci_find(name, query);
        proper_name_match = ci_find(proper_name, query);

        bool matches = name_match != std::string::npos || proper_name_match != std::string::npos;

        if (!matches)
          continue;
      }

      shown_count++;

      ImGui::PushID((name + version + module_path).c_str());

      float row_start_y = ImGui::GetCursorPosY();

      ImGui::BeginGroup();

      ImGui::Image(Cherry::GetTexture(module->logo_path()), ImVec2(48.0f, 48.0f));
      ImGui::SameLine(0.0f, 12.0f);

      ImGui::BeginGroup();
      {
        draw_highlighted_text(proper_name.empty() ? name : proper_name, proper_name_match, query.size());
        ImGui::SameLine();
        ImGui::TextDisabled("(%s)", version.c_str());

        if (!group.empty()) {
          ImGui::SameLine();
          ImGui::TextDisabled("- %s", group.c_str());
        }

        if (!description.empty()) {
          ImGui::Dummy(ImVec2(0.0f, 2.0f));
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 1.0f));
          ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - button_width);
          ImGui::TextUnformatted(description.c_str());
          ImGui::PopTextWrapPos();
          ImGui::PopStyleColor();
        }

        ImGui::Dummy(ImVec2(0.0f, 3.0f));

        ImGui::SetWindowFontScale(0.85f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
        ImGui::TextUnformatted(folder_name.c_str());
        ImGui::PopStyleColor();
        ImGui::SetWindowFontScale(1.0f);

        if (!author.empty()) {
          ImGui::SetWindowFontScale(0.78f);
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.42f, 0.42f, 0.42f, 1.0f));
          ImGui::Text("from %s", author.c_str());
          ImGui::PopStyleColor();
          ImGui::SetWindowFontScale(1.0f);
        }
      }
      ImGui::EndGroup();

      ImGui::EndGroup();

      float group_height = ImGui::GetItemRectSize().y;
      float button_y = row_start_y + (group_height - ImGui::GetFrameHeight()) * 0.5f;

      ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - button_width);
      ImGui::SetCursorPosY(button_y);

      if (already_installed) {
        ImGui::BeginDisabled();
        ImGui::Button("Installed", ImVec2(80.0f, 0.0f));
        ImGui::EndDisabled();
      } else {
        if (ImGui::Button("Install", ImVec2(80.0f, 0.0f))) {
          bool restart = restart_after_install;
          vxe::install_module_by_path(name, module_path, restart);

          feedback_message = "Module \"" + (proper_name.empty() ? name : proper_name) + "\" installed.";
        }
      }

      ImGui::PopID();

      ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 1.0f, 1.0f, 0.08f));
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 30.0f);
      ImGui::Separator();
      ImGui::PopStyleColor();
    }

    if (!query.empty() && shown_count == 0) {
      ImGui::TextDisabled("No modules correspond to \"%s\".", query.c_str());
    }

    ImGui::EndChild();

    if (!feedback_message.empty()) {
      ImGui::Dummy(ImVec2(0.0f, 4.0f));
      ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.4f, 1.0f), "%s", feedback_message.c_str());
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleColor();
  }

}  // namespace vxe