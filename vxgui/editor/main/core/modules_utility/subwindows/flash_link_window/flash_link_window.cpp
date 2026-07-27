//
//  flash_link_window.cpp
//  Sources for the "flash link window" of the modules utility
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./flash_link_window.hpp"

namespace vxe {

  namespace {

    struct TileEntry {
      std::string id;
      ImTextureID icon;
      std::string title;
      bool enabled = true;
      std::function<void()> on_click;
    };

    bool MatchesSearch(const std::string &title, const std::string &search) {
      if (search.empty()) {
        return true;
      }
      std::string lowerTitle = title;
      std::string lowerSearch = search;
      std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
      std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
      return lowerTitle.find(lowerSearch) != std::string::npos;
    }

    bool DrawTile(const char *id, ImTextureID icon, const std::string &title, float w, float h, bool enabled) {
      ImVec2 pos = CherryGUI::GetCursorScreenPos();
      ImDrawList *dl = CherryGUI::GetWindowDrawList();

      bool clicked = false;
      bool hovered = false;

      if (enabled) {
        clicked = CherryGUI::InvisibleButton(id, ImVec2(w, h));
        hovered = CherryGUI::IsItemHovered();
        if (hovered) {
          CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
      } else {
        CherryGUI::Dummy(ImVec2(w, h));
      }

      ImU32 bg = enabled ? (hovered ? IM_COL32(58, 58, 58, 255) : IM_COL32(40, 40, 40, 255)) : IM_COL32(30, 30, 30, 255);
      ImU32 border =
          enabled ? (hovered ? IM_COL32(90, 130, 220, 255) : IM_COL32(65, 65, 65, 255)) : IM_COL32(50, 50, 50, 255);
      ImU32 textColor = enabled ? IM_COL32(215, 215, 215, 255) : IM_COL32(110, 110, 110, 255);

      dl->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h), bg, 5.0f);
      dl->AddRect(pos, ImVec2(pos.x + w, pos.y + h), border, 5.0f, 0, 1.2f);

      float iconSize = 28.0f;
      ImVec2 iconPos(pos.x + (w - iconSize) * 0.5f, pos.y + 12.0f);
      if (icon) {
        ImVec4 tint = enabled ? ImVec4(1, 1, 1, 1) : ImVec4(1, 1, 1, 0.35f);
        dl->AddImage(
            icon,
            iconPos,
            ImVec2(iconPos.x + iconSize, iconPos.y + iconSize),
            ImVec2(0, 0),
            ImVec2(1, 1),
            CherryGUI::ColorConvertFloat4ToU32(tint));
      }

      ImVec2 textSize = CherryGUI::CalcTextSize(title.c_str());
      float textX = pos.x + (w - textSize.x) * 0.5f;
      if (textSize.x > w - 8.0f) {
        textX = pos.x + 4.0f;
      }
      ImVec2 textPos(textX, iconPos.y + iconSize + 8.0f);
      dl->AddText(textPos, textColor, title.c_str());

      return clicked;
    }

    void
    DrawTileGrid(const std::vector<TileEntry> &entries, float available_width, float min_tile_w, float tile_h, float gap) {
      if (entries.empty()) {
        return;
      }

      int columns = (int)((available_width + gap) / (min_tile_w + gap));
      if (columns < 1) {
        columns = 1;
      }
      if (columns > (int)entries.size()) {
        columns = (int)entries.size();
      }

      float tile_w = (available_width - (columns - 1) * gap) / columns;

      for (size_t i = 0; i < entries.size(); ++i) {
        const auto &e = entries[i];

        if (DrawTile(e.id.c_str(), e.icon, e.title, tile_w, tile_h, e.enabled)) {
          if (e.on_click) {
            e.on_click();
          }
        }

        bool lastInRow = ((i + 1) % columns == 0);
        bool lastEntry = (i + 1 == entries.size());

        if (!lastInRow && !lastEntry) {
          CherryGUI::SameLine(0.0f, gap);
        } else if (!lastEntry) {
          CherryGUI::Dummy(ImVec2(0.0f, gap));
        }
      }

      CherryGUI::Dummy(ImVec2(0.0f, gap));
    }

    bool DrawRow(const char *id, ImTextureID icon, const std::string &title, const std::string &description, float width) {
      float height = 56.0f;
      float padding = 10.0f;
      float iconSize = 34.0f;

      ImVec2 pos = CherryGUI::GetCursorScreenPos();
      ImDrawList *dl = CherryGUI::GetWindowDrawList();

      bool clicked = CherryGUI::InvisibleButton(id, ImVec2(width, height));
      bool hovered = CherryGUI::IsItemHovered();

      if (hovered) {
        CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
      }

      ImU32 bg = hovered ? IM_COL32(52, 52, 52, 255) : IM_COL32(38, 38, 38, 255);
      dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), bg, 5.0f);

      ImVec2 iconPos(pos.x + padding, pos.y + (height - iconSize) * 0.5f);
      if (icon) {
        dl->AddImage(icon, iconPos, ImVec2(iconPos.x + iconSize, iconPos.y + iconSize));
      }

      float textX = iconPos.x + iconSize + padding;
      dl->AddText(ImVec2(textX, pos.y + 9.0f), IM_COL32(230, 230, 230, 255), title.c_str());
      dl->AddText(ImVec2(textX, pos.y + 30.0f), IM_COL32(150, 150, 150, 255), description.c_str());

      return clicked;
    }

  }  // namespace

  FlashLinkWindow::FlashLinkWindow(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [this]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(8.0f);
    app_window_->SetInternalPaddingY(8.0f);

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::shared_ptr<Cherry::AppWindow> &FlashLinkWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<FlashLinkWindow> FlashLinkWindow::create(const std::string &name) {
    auto instance = std::shared_ptr<FlashLinkWindow>(new FlashLinkWindow(name));
    instance->setup_render_callback();
    return instance;
  }

  void FlashLinkWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void FlashLinkWindow::startSearch(const std::string &flashlink) {
    uint64_t my_token = ++search_token;

    current_flashlink = flashlink;
    state = FlashLinkState::Loading;

    std::thread([this, my_token]() {
      while (search_token.load() == my_token) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
    }).detach();
  }

  void FlashLinkWindow::render() {
    if (!clipboard_checked) {
      clipboard_checked = true;

      const char *clipboard = ImGui::GetClipboardText();
      if (clipboard != nullptr) {
        std::string clip_str(clipboard);

        if (!clip_str.empty() && clip_str.length() < 70) {
          bool all_valid = true;
          for (unsigned char c : clip_str) {
            if (!is_base64(c) && c != '=') {
              all_valid = false;
              break;
            }
          }

          if (all_valid) {
            detected = true;
            decoded_text = base64_decode(clip_str);

            if (is_module_flashlink(decoded_text)) {
              startSearch(decoded_text);
            }
          } else {
            detected = false;
          }
        }
      }
    }

    ImGui::TextUnformatted("Paste flashlink");

    bool edited = ImGui::InputText("##flashlink_input", input_buffer, IM_ARRAYSIZE(input_buffer));

    if (edited) {
      std::string typed(input_buffer);

      if (!typed.empty() && typed.length() < 70) {
        bool all_valid = true;
        for (unsigned char c : typed) {
          if (!is_base64(c) && c != '=') {
            all_valid = false;
            break;
          }
        }

        if (all_valid) {
          std::string decoded = base64_decode(typed);

          if (is_module_flashlink(decoded)) {
            detected = true;
            decoded_text = decoded;

            startSearch(decoded);

            input_buffer[0] = '\0';
          }
        }
      }
    }

    ImGui::Separator();

    switch (state) {
      case FlashLinkState::WaitingForClipboard: {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No flashlink for module detected");
        ImGui::TextWrapped("Please enter a valid flashlink");
        break;
      }

      case FlashLinkState::Loading: {
        static const char *frames[] = { "|", "/", "-", "\\" };
        int frame = static_cast<int>(ImGui::GetTime() / 0.15f) % 4;

        ImGui::Text("Searching %s", frames[frame]);
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Flashlink : %s", current_flashlink.c_str());
        break;
      }

      case FlashLinkState::Ready: {
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Search terminated !");
        ImGui::Text("Flashlink : %s", current_flashlink.c_str());
        break;
      }
    }
  }

}  // namespace vxe