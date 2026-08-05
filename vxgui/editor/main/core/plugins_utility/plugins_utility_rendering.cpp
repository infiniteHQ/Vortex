//
//  plugins_utility_rendering.cpp
//  Sources of rendering and UI related functions
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./plugins_utility.hpp"

namespace vxe {

  void PluginsUtility::render() {
    bool redocked_this_frame = false;

    if (!focus_window_applied_) {
      if (!focus_window_.empty()) {
        for (auto &w : CherryApp.GetWindows()) {
          if (w && w->GetName() == focus_window_) {
            CherryApp.QuickRedock(app_window_->GetName(), w->GetName());
            redocked_this_frame = true;
            break;
          }
        }
      }
      focus_window_applied_ = true;
    }

    if (focus_pending_) {
      if (redocked_this_frame) {
        wait_one_frame_for_focus_ = true;
      } else if (wait_one_frame_for_focus_) {
        wait_one_frame_for_focus_ = false;
        CherryApp.FocusAppWindow(app_window_->GetName());
        focus_pending_ = false;
      } else {
        CherryApp.FocusAppWindow(app_window_->GetName());
        focus_pending_ = false;
      }
    }

    render_plugin_deletion_modal();
    if (selected_pannel_ == PluginsUtilityPannels::Installed) {
      render_installed();
    } else if (selected_pannel_ == PluginsUtilityPannels::Import) {
      render_import();
    } else if (selected_pannel_ == PluginsUtilityPannels::Downloads) {
      render_download();
    }
  }

  void PluginsUtility::render_net_permission_modal() {
    if (trigger_net_permission_modal_) {
      CherryGUI::OpenPopup("##net_permission_modal");
    }

    ImVec2 center = CherryGUI::GetMainViewport()->GetCenter();
    CherryGUI::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    CherryGUI::SetNextWindowSize(ImVec2(460, 0), ImGuiCond_Appearing);
    CherryGUI::SetNextWindowBgAlpha(1.0f);

    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    CherryGUI::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.00f, 0.00f, 0.00f, 0.65f));
    CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.22f, 0.24f, 1.00f));

    if (CherryGUI::BeginPopupModal(
            "##net_permission_modal",
            &trigger_net_permission_modal_,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
      const float MODAL_W = 460.0f;

      CherryGUI::BeginChild(
          "##net_topbar", ImVec2(MODAL_W, 48.0f), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
      {
        CherryGUI::SetCursorPos(ImVec2(16.0f, 13.0f));
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        CherryGUI::TextUnformatted("Allow net features");
        CherryGUI::PopStyleColor();
      }
      CherryGUI::EndChild();

      CherryGUI::SetCursorPosX(0.0f);
      CherryGUI::BeginChild(
          "##net_body",
          ImVec2(MODAL_W, 150.0f),
          false,
          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize);
      {
        CherryGUI::SetCursorPos(ImVec2(20.0f, 16.0f));
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.62f, 0.62f, 0.65f, 1.0f));
        CherryGUI::PushTextWrapPos(CherryGUI::GetCursorPosX() + MODAL_W - 40.0f);
        CherryGUI::TextUnformatted(
            "Enabling net features allows Vortex to perform network requests. "
            "However, Vortex will never share data of any kind with anyone. This only "
            "enables searching public sources. Once enabled, you can disable this feature "
            "from the project settings window.");
        CherryGUI::PopTextWrapPos();
        CherryGUI::PopStyleColor();

        CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 14.0f);
        ImDrawList *dl = CherryGUI::GetWindowDrawList();
        ImVec2 sepA = CherryGUI::GetCursorScreenPos();
        dl->AddLine(sepA, ImVec2(sepA.x + MODAL_W, sepA.y), IM_COL32(50, 50, 55, 255), 1.0f);
        CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 1.0f);

        const float BTN_H = 36.0f;
        const float BTN_W = 140.0f;
        const float PAD = 16.0f;

        CherryGUI::SetCursorPos(ImVec2(MODAL_W - (BTN_W * 2.0f + 8.0f + PAD), CherryGUI::GetCursorPosY() + 12.0f));

        CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.20f, 0.22f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.26f, 0.28f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.17f, 0.17f, 0.19f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.78f, 0.78f, 0.80f, 1.0f));

        if (CherryGUI::Button("Cancel", ImVec2(BTN_W, BTN_H))) {
          trigger_net_permission_modal_ = false;
          CherryGUI::CloseCurrentPopup();
        }
        CherryGUI::PopStyleColor(4);

        CherryGUI::SameLine(0.0f, 8.0f);

        CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.45f, 0.20f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.55f, 0.26f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.38f, 0.16f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        if (CherryGUI::Button("Allow net", ImVec2(BTN_W, BTN_H))) {
          vxe::toggle_vortex_net_permission(
              [](const std::string &err) { std::cerr << "[render_net_permission_modal] " << err << std::endl; });
          // toggle_vortex_net_permission refresh the ctx automaticly
          trigger_net_permission_modal_ = false;
          CherryGUI::CloseCurrentPopup();
        }
        CherryGUI::PopStyleColor(4);
        CherryGUI::PopStyleVar();

        CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 14.0f);
      }
      CherryGUI::EndChild();

      CherryGUI::EndPopup();
    }

    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(3);
  }

  void PluginsUtility::render_plugin_deletion_modal() {
    if (trigger_plugin_deletion_modal_) {
      CherryGUI::OpenPopup("##delete_plugin_modal");
    }

    ImVec2 center = CherryGUI::GetMainViewport()->GetCenter();
    CherryGUI::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    CherryGUI::SetNextWindowSize(ImVec2(520, 0), ImGuiCond_Appearing);
    CherryGUI::SetNextWindowBgAlpha(1.0f);

    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.11f, 0.11f, 0.12f, 1.00f));
    CherryGUI::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.00f, 0.00f, 0.00f, 0.65f));
    CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.22f, 0.24f, 1.00f));

    if (CherryGUI::BeginPopupModal(
            "##delete_plugin_modal",
            &trigger_plugin_deletion_modal_,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
      const float MODAL_W = 520.0f;

      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.62f, 0.10f, 0.10f, 1.0f));
      CherryGUI::BeginChild("##topbar", ImVec2(MODAL_W, 48.0f), false, ImGuiWindowFlags_NoScrollbar);
      {
        CherryGUI::SetCursorPos(ImVec2(16.0f, 13.0f));
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        CherryGUI::TextUnformatted("Delete plugin");
        CherryGUI::PopStyleColor();
      }
      CherryGUI::EndChild();
      CherryGUI::PopStyleColor();

      CherryGUI::SetCursorPosX(0.0f);
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
      CherryGUI::BeginChild(
          "##body", ImVec2(MODAL_W, 200.0f), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
      {
        CherryGUI::SetCursorPos(ImVec2(20.0f, 16.0f));

        CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16f, 0.16f, 0.18f, 1.0f));
        CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
        CherryGUI::SetCursorPosX(20.0f);
        CherryGUI::BeginChild("##plugincard", ImVec2(MODAL_W - 40.0f, 82.0f), false, ImGuiWindowFlags_NoScrollbar);
        {
          const float LOGO_SIZE = 52.0f;
          CherryGUI::SetCursorPos(ImVec2(14.0f, 15.0f));

          ImTextureID logo = Cherry::GetTexture(plugin_to_delete_logo_path_);
          if (logo) {
            ImVec2 logoSize = Cherry::GetTextureSize(plugin_to_delete_logo_path_);
            float scale = LOGO_SIZE / (std::max)(logoSize.x, logoSize.y);
            ImVec2 drawSize(logoSize.x * scale, logoSize.y * scale);
            ImVec2 imgPos = CherryGUI::GetCursorPos();
            imgPos.x += (LOGO_SIZE - drawSize.x) * 0.5f;
            imgPos.y += (LOGO_SIZE - drawSize.y) * 0.5f;
            CherryGUI::SetCursorPos(imgPos);
            CherryGUI::Image(logo, drawSize);
          } else {
            ImVec2 p = CherryGUI::GetCursorScreenPos();
            ImDrawList *dl = CherryGUI::GetWindowDrawList();
            dl->AddRectFilled(p, ImVec2(p.x + LOGO_SIZE, p.y + LOGO_SIZE), IM_COL32(80, 40, 40, 255), 8.0f);
            std::string initials =
                plugin_to_delete_proper_name_.size() >= 2 ? plugin_to_delete_proper_name_.substr(0, 2) : "??";
            CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.6f, 1.0f));
            ImVec2 ts = CherryGUI::CalcTextSize(initials.c_str());
            CherryGUI::SetCursorScreenPos(ImVec2(p.x + (LOGO_SIZE - ts.x) * 0.5f, p.y + (LOGO_SIZE - ts.y) * 0.5f));
            CherryGUI::TextUnformatted(initials.c_str());
            CherryGUI::PopStyleColor();
            CherryGUI::SetCursorPos(ImVec2(14.0f + LOGO_SIZE + 14.0f, 15.0f));
          }

          float textX = 14.0f + LOGO_SIZE + 14.0f;
          CherryGUI::SetCursorPos(ImVec2(textX, 15.0f));

          CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.96f, 1.0f));
          CherryGUI::TextUnformatted(plugin_to_delete_proper_name_.c_str());
          CherryGUI::PopStyleColor();

          CherryGUI::SetCursorPos(ImVec2(textX, 36.0f));
          CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.58f, 1.0f));
          CherryGUI::Text("v%s", plugin_to_delete_version_.c_str());
          CherryGUI::PopStyleColor();

          if (!plugin_to_delete_description_.empty()) {
            CherryGUI::SetCursorPos(ImVec2(textX, 54.0f));
            CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.45f, 0.48f, 1.0f));
            // Clamp to single line with ellipsis
            std::string desc = plugin_to_delete_description_;
            const float maxW = MODAL_W - 40.0f - textX - 14.0f;
            while (!desc.empty() && CherryGUI::CalcTextSize((desc + "...").c_str()).x > maxW)
              desc.pop_back();
            if (desc.size() < plugin_to_delete_description_.size())
              desc += "...";
            CherryGUI::TextUnformatted(desc.c_str());
            CherryGUI::PopStyleColor();
          }
        }
        CherryGUI::EndChild();
        CherryGUI::PopStyleVar();
        CherryGUI::PopStyleColor();

        CherryGUI::SetCursorPos(ImVec2(20.0f, CherryGUI::GetCursorPosY() + 8.0f));
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.62f, 0.62f, 0.65f, 1.0f));
        CherryGUI::PushTextWrapPos(CherryGUI::GetCursorPosX() + MODAL_W - 40.0f);
        CherryGUI::TextUnformatted(
            "This action is permanent. The plugin files "
            "will be removed from disk "
            "and cannot be recovered.");
        CherryGUI::PopTextWrapPos();
        CherryGUI::PopStyleColor();

        CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 14.0f);
        ImDrawList *dl = CherryGUI::GetWindowDrawList();
        ImVec2 sepA = CherryGUI::GetCursorScreenPos();
        dl->AddLine(sepA, ImVec2(sepA.x + MODAL_W, sepA.y), IM_COL32(50, 50, 55, 255), 1.0f);
        CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 1.0f);

        const float BTN_H = 36.0f;
        const float BTN_W = 140.0f;
        const float PAD = 16.0f;

        CherryGUI::SetCursorPos(ImVec2(MODAL_W - (BTN_W * 2.0f + 8.0f + PAD), CherryGUI::GetCursorPosY() + 12.0f));

        CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
        CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.20f, 0.22f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.26f, 0.28f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.17f, 0.17f, 0.19f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.78f, 0.78f, 0.80f, 1.0f));

        if (CherryGUI::Button("Cancel", ImVec2(BTN_W, BTN_H))) {
          plugin_to_delete_name_.clear();
          plugin_to_delete_proper_name_.clear();
          plugin_to_delete_description_.clear();
          plugin_to_delete_version_.clear();
          plugin_to_delete_logo_path_.clear();
          trigger_plugin_deletion_modal_ = false;
          CherryGUI::CloseCurrentPopup();
        }
        CherryGUI::PopStyleColor(4);

        CherryGUI::SameLine(0.0f, 8.0f);

        CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.62f, 0.10f, 0.10f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.14f, 0.14f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.50f, 0.08f, 0.08f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        if (CherryGUI::Button("Delete plugin", ImVec2(BTN_W, BTN_H))) {
          vxe::delete_project_plugin(plugin_to_delete_name_, plugin_to_delete_version_);
          vxe::load_editor_plugins(
              vxe::get_current_context()->projectPath.string(),
              vxe::get_current_context()->IO.ep_handles,
              vxe::get_current_context()->IO.ep);
          plugin_to_delete_name_.clear();
          plugin_to_delete_proper_name_.clear();
          plugin_to_delete_description_.clear();
          plugin_to_delete_version_.clear();
          plugin_to_delete_logo_path_.clear();
          selected_category_changed_ = true;
          refresh_categories();
          CherryGUI::CloseCurrentPopup();
        }
        CherryGUI::PopStyleColor(4);
        CherryGUI::PopStyleVar();

        CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 14.0f);
      }
      CherryGUI::EndChild();
      CherryGUI::PopStyleColor();

      CherryGUI::EndPopup();
    }

    CherryGUI::PopStyleColor(3);
    CherryGUI::PopStyleVar(3);
  }

  void PluginsUtility::render_download() {
    ImGuiIO &io = CherryGUI::GetIO();
    ImVec2 windowSize = CherryGUI::GetContentRegionAvail();
    ImVec2 windowPos = CherryGUI::GetCursorScreenPos();

    float centerX = windowPos.x + windowSize.x * 0.5f;
    float centerY = windowPos.y + windowSize.y * 0.5f;

    ImTextureID logo = Cherry::Application::Get().GetCurrentRenderedWindow()->GetTexture(
        Cherry::GetPath("resources/imgs/infinite_garage.png"));

    float logoWidth = 214.0f;
    float logoHeight = 100.0f;
    float totalBlockHeight = logoHeight + 20.0f + 36.0f + 12.0f + 60.0f + 20.0f + 28.0f + 80.0f;

    float startY = centerY - totalBlockHeight * 0.5f;

    CherryGUI::SetCursorScreenPos(ImVec2(centerX - logoWidth * 0.5f, startY));
    CherryGUI::Image(logo, ImVec2(logoWidth, logoHeight));

    Cherry::PushFont("JetBrainsMono");
    const char *title = "You will be able to download content from the Infinite Garage";
    ImVec2 titleSize = CherryGUI::CalcTextSize(title);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - titleSize.x * 0.5f, startY + logoHeight + 20.0f));
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    CherryGUI::Text("%s", title);
    Cherry::PopFont();

    const char *badge = "  WIP  ";
    ImVec2 badgeSize = CherryGUI::CalcTextSize(badge);
    float badgePaddingX = 16.0f;
    float badgePaddingY = 6.0f;
    float badgeTotalW = badgeSize.x + badgePaddingX * 2.0f;
    float badgeTotalH = badgeSize.y + badgePaddingY * 2.0f;
    float badgeX = centerX - badgeTotalW * 0.5f;
    float badgeY = startY + logoHeight + 20.0f + 36.0f + 12.0f;

    ImDrawList *draw = CherryGUI::GetWindowDrawList();
    draw->AddRectFilled(
        ImVec2(badgeX, badgeY), ImVec2(badgeX + badgeTotalW, badgeY + badgeTotalH), IM_COL32(47, 47, 47, 255), 6.0f);
    CherryGUI::SetCursorScreenPos(ImVec2(badgeX + badgePaddingX, badgeY + badgePaddingY));
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    CherryGUI::Text("%s", badge);
    CherryGUI::PopStyleColor();

    float sepY = badgeY + badgeTotalH + 20.0f;
    draw->AddLine(ImVec2(centerX - 120.0f, sepY), ImVec2(centerX + 120.0f, sepY), IM_COL32(255, 255, 255, 30), 1.0f);

    const char *desc = "Download & content management is currently in development.";
    const char *desc2 = "The Infinite Garage is a sharing platform for makers.";

    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.65f, 0.75f, 1.0f));

    ImVec2 d1 = CherryGUI::CalcTextSize(desc);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - d1.x * 0.5f, sepY + 16.0f));
    CherryGUI::Text("%s", desc);

    ImVec2 d2 = CherryGUI::CalcTextSize(desc2);
    float desc2Y = sepY + 16.0f + d1.y + 6.0f;
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - d2.x * 0.5f, desc2Y));
    CherryGUI::Text("%s", desc2);

    CherryGUI::PopStyleColor(2);

    float sep2Y = desc2Y + d2.y + 22.0f;
    draw->AddLine(ImVec2(centerX - 200.0f, sep2Y), ImVec2(centerX + 200.0f, sep2Y), IM_COL32(255, 255, 255, 18), 1.0f);

    float tipY = sep2Y + 14.0f;

    const char *tipTitle = "Manual installation";
    ImVec2 tipTitleSize = CherryGUI::CalcTextSize(tipTitle);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - tipTitleSize.x * 0.5f, tipY));
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.78f, 0.40f, 1.0f));
    CherryGUI::Text("%s", tipTitle);
    CherryGUI::PopStyleColor();

    const char *tip1 = "You can still browse & download content from the Garage,";
    const char *tip2 = "then import it manually into your project's  .vx/plugins/  folder.";
    const char *tip3 = "For more information, refer to the documentation.";

    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.58f, 0.58f, 0.68f, 1.0f));

    ImVec2 t1 = CherryGUI::CalcTextSize(tip1);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - t1.x * 0.5f, tipY + 22.0f));
    CherryGUI::Text("%s", tip1);

    ImVec2 t2 = CherryGUI::CalcTextSize(tip2);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - t2.x * 0.5f, tipY + 22.0f + t1.y + 4.0f));
    CherryGUI::Text("%s", tip2);

    ImVec2 t3 = CherryGUI::CalcTextSize(tip3);
    float tip3Y = tipY + 22.0f + t1.y + 4.0f + t2.y + 4.0f;
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - t3.x * 0.5f, tip3Y));
    CherryGUI::Text("%s", tip3);

    CherryGUI::PopStyleColor();

    if (true) {
      float btnW = 140.0f;
      float btnH = 34.0f;
      float btnY = tip3Y + t3.y + 28.0f;

      CherryGUI::SetCursorScreenPos(ImVec2(centerX - btnW * 0.5f, btnY));

      CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.38f, 0.40f, 0.95f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.48f, 0.50f, 1.0f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.28f, 0.30f, 0.80f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

      if (CherryKit::ButtonImageTextImage(
              "Browse the Garage",
              Cherry::GetPath("resources/imgs/icons/misc/icon_net.png"),
              Cherry::GetPath("resources/imgs/icons/misc/icon_redirect.png"))
              .GetDataAs<bool>("isClicked")) {
        vxe::open_url("https://garage.infinite.si");
      }

      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor(4);
    }
  }

  void PluginsUtility::render_import() {
    ImGuiIO &io = CherryGUI::GetIO();
    ImVec2 windowSize = CherryGUI::GetContentRegionAvail();
    ImVec2 windowPos = CherryGUI::GetCursorScreenPos();

    float centerX = windowPos.x + windowSize.x * 0.5f;
    float centerY = windowPos.y + windowSize.y * 0.5f;

    float totalBlockHeight = 20.0f + 36.0f + 12.0f + 60.0f + 20.0f + 28.0f + 80.0f;

    float startY = centerY - totalBlockHeight * 0.5f;

    CherryGUI::SetCursorScreenPos(ImVec2(centerX * 0.5f, startY));

    Cherry::PushFont("JetBrainsMono");
    const char *title = "You will be able to import contents directly here";
    ImVec2 titleSize = CherryGUI::CalcTextSize(title);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - titleSize.x * 0.5f, startY + 20.0f));
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    CherryGUI::Text("%s", title);
    Cherry::PopFont();

    const char *badge = "  WIP  ";
    ImVec2 badgeSize = CherryGUI::CalcTextSize(badge);
    float badgePaddingX = 16.0f;
    float badgePaddingY = 6.0f;
    float badgeTotalW = badgeSize.x + badgePaddingX * 2.0f;
    float badgeTotalH = badgeSize.y + badgePaddingY * 2.0f;
    float badgeX = centerX - badgeTotalW * 0.5f;
    float badgeY = startY + 20.0f + 36.0f + 12.0f;

    ImDrawList *draw = CherryGUI::GetWindowDrawList();
    draw->AddRectFilled(
        ImVec2(badgeX, badgeY), ImVec2(badgeX + badgeTotalW, badgeY + badgeTotalH), IM_COL32(47, 47, 47, 255), 6.0f);
    CherryGUI::SetCursorScreenPos(ImVec2(badgeX + badgePaddingX, badgeY + badgePaddingY));
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    CherryGUI::Text("%s", badge);
    CherryGUI::PopStyleColor();

    float sepY = badgeY + badgeTotalH + 20.0f;
    draw->AddLine(ImVec2(centerX - 120.0f, sepY), ImVec2(centerX + 120.0f, sepY), IM_COL32(255, 255, 255, 30), 1.0f);

    const char *desc = "Quick importation utility is currently in development.";

    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.65f, 0.75f, 1.0f));

    ImVec2 d1 = CherryGUI::CalcTextSize(desc);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - d1.x * 0.5f, sepY + 16.0f));
    CherryGUI::Text("%s", desc);

    CherryGUI::PopStyleColor(2);

    float sep2Y = badgeY + badgeTotalH + 122.0f;
    draw->AddLine(ImVec2(centerX - 200.0f, sep2Y), ImVec2(centerX + 200.0f, sep2Y), IM_COL32(255, 255, 255, 18), 1.0f);

    float tipY = sep2Y + 14.0f;

    const char *tipTitle = "Manual installation";
    ImVec2 tipTitleSize = CherryGUI::CalcTextSize(tipTitle);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - tipTitleSize.x * 0.5f, tipY));
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.78f, 0.40f, 1.0f));
    CherryGUI::Text("%s", tipTitle);
    CherryGUI::PopStyleColor();

    const char *tip1 = "You can still import manually into your project's  .vx/plugins/  folder";
    const char *tip3 = "For more information, refer to the documentation.";

    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.58f, 0.58f, 0.68f, 1.0f));

    ImVec2 t1 = CherryGUI::CalcTextSize(tip1);
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - t1.x * 0.5f, tipY + 22.0f));
    CherryGUI::Text("%s", tip1);

    ImVec2 t3 = CherryGUI::CalcTextSize(tip3);
    float tip3Y = tipY + 22.0f + t1.y + 4.0f + 4.0f;
    CherryGUI::SetCursorScreenPos(ImVec2(centerX - t3.x * 0.5f, tip3Y));
    CherryGUI::Text("%s", tip3);

    CherryGUI::PopStyleColor();
  }

  void PluginsUtility::render_plugin_list_row(const std::shared_ptr<PluginInterface> &plu) {
    if (!plu) {
      return;
    }

    const float ROW_H = 78.0f;
    const float LOGO_SIZE = 44.0f;
    const float BTN_SIZE = 22.0f;
    const float BTN_SPACING = 8.0f;
    const float PADDING = 6.0f;

    std::string row_id = "##plugin_row_" + plu->name() + "_" + plu->version();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.14f, 0.14f, 0.15f, 1.0f));
    CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.20f, 0.20f, 0.22f, 1.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    CherryGUI::BeginChild(row_id.c_str(), ImVec2(-FLT_MIN, ROW_H), true, ImGuiWindowFlags_NoScrollbar);
    {
      ImVec2 rowPos = CherryGUI::GetCursorScreenPos();
      ImVec2 rowSize = CherryGUI::GetContentRegionAvail();
      ImDrawList *dl = CherryGUI::GetWindowDrawList();

      float logoY = rowPos.y + (ROW_H - LOGO_SIZE) * 0.5f - CherryGUI::GetScrollY() - 20.0f;
      ImTextureID logo = Cherry::GetTexture(plu->logo_path());
      if (logo) {
        ImVec2 logoSize = Cherry::GetTextureSize(plu->logo_path());
        float scale = LOGO_SIZE / (std::max)(logoSize.x, logoSize.y);
        ImVec2 drawSize(logoSize.x * scale, logoSize.y * scale);
        ImVec2 imgPos(rowPos.x + PADDING + (LOGO_SIZE - drawSize.x) * 0.5f, logoY + (LOGO_SIZE - drawSize.y) * 0.5f);
        dl->AddImage(logo, imgPos, ImVec2(imgPos.x + drawSize.x, imgPos.y + drawSize.y));
      } else {
        ImVec2 p(rowPos.x + PADDING, logoY);
        dl->AddRectFilled(p, ImVec2(p.x + LOGO_SIZE, p.y + LOGO_SIZE), IM_COL32(60, 60, 64, 255), 6.0f);
        std::string initials = plu->proper_name().size() >= 2 ? plu->proper_name().substr(0, 2) : "??";
        ImVec2 ts = CherryGUI::CalcTextSize(initials.c_str());
        dl->AddText(
            ImVec2(p.x + (LOGO_SIZE - ts.x) * 0.5f, p.y + (LOGO_SIZE - ts.y) * 0.5f),
            IM_COL32(200, 200, 205, 255),
            initials.c_str());
      }

      float btnAreaW = BTN_SIZE * 3.0f + BTN_SPACING * 2.0f;
      float btnX = rowPos.x + rowSize.x - PADDING - BTN_SIZE;
      float btnY = rowPos.y + (ROW_H - BTN_SIZE) * 0.5f - 26.0f;

      CherryGUI::SetCursorScreenPos(ImVec2(btnX, btnY));
      if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
              .GetDataAs<bool>("isClicked")) {
        std::string label = "Details of " + plu->proper_name() + "####" + plu->path();
        // std::shared_ptr<PluginDetails> window = PluginDetails::Create(label, plu);
        // Cherry::AddAppWindow(window->GetAppWindow());
      }

      btnX -= (BTN_SIZE + BTN_SPACING);
      CherryGUI::SetCursorScreenPos(ImVec2(btnX, btnY));
      if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png")).GetDataAs<bool>("isClicked")) {
        plu->stop();
        set_plugin_to_delete(plu);
      }

      btnX -= (BTN_SIZE + BTN_SPACING);
      CherryGUI::SetCursorScreenPos(ImVec2(btnX, btnY));
      if (plu->state() == "failed") {
        if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_retry.png"))
                .GetDataAs<bool>("isClicked")) {
          plu->start();
        }
      } else if (plu->state() == "unknow" || plu->state() == "stopped") {
        if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_start.png"))
                .GetDataAs<bool>("isClicked")) {
          plu->start();
        }
      } else {
        if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_stop.png"))
                .GetDataAs<bool>("isClicked")) {
          plu->stop();
        }
      }

      float textX = rowPos.x + PADDING + LOGO_SIZE + 16.0f;
      float textMaxW = rowSize.x - (textX - rowPos.x) - btnAreaW - PADDING - 12.0f;
      if (textMaxW < 80.0f) {
        textMaxW = 80.0f;
      }

      std::string title = plu->proper_name();
      ImVec2 titlePos(textX, rowPos.y);
      dl->AddText(titlePos, IM_COL32(240, 240, 242, 255), title.c_str());
      ImVec2 titleSize = CherryGUI::CalcTextSize(title.c_str());

      std::string ver = "  v" + plu->version();
      dl->AddText(ImVec2(titlePos.x + titleSize.x, titlePos.y), IM_COL32(140, 140, 145, 255), ver.c_str());

      CherryGUI::SetCursorScreenPos(ImVec2(textX, rowPos.y + 22.0f));
      CherryGUI::PushTextWrapPos(textX + textMaxW);
      CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.58f, 0.58f, 0.62f, 1.0f));
      CherryGUI::PushItemWidth(textMaxW);
      CherryGUI::TextWrapped("%s", plu->description().c_str());
      CherryGUI::PopItemWidth();
      CherryGUI::PopStyleColor();
      CherryGUI::PopTextWrapPos();
    }
    CherryGUI::EndChild();

    CherryGUI::PopStyleVar(2);
    CherryGUI::PopStyleColor(2);
  }

  void PluginsUtility::render_installed() {
    const float minPaneWidth = 50.0f;
    const float splitterWidth = 1.5f;

    std::string label = "left_pane" + app_window_->m_Name;
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::BeginChild(label.c_str(), ImVec2(left_panel_width, 0), true, 0);

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);
    CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/banner_modules.png")), ImVec2(280, 142));

    Cherry::PushFont("Clash");
    CherryStyle::RemoveMarginY(40.0f);
    CherryStyle::AddMarginX(15.0f);
    CherryStyle::PushFontSize(0.28f);
    CherryKit::TitleOne("Plugins");
    CherryStyle::PopFontSize();
    Cherry::PopFont();

    // CherryStyle::SetPadding(7.0f);

    const float input_width = left_panel_width - 17.0f;
    const float header_width = left_panel_width - 27.0f;

    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 7));

    CherryKit::Space(3.0f);
    CherryStyle::AddMarginX(6.0f);
    if (selected_category_ == "all") {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#787878"));
      CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#454545"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#565656"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
      CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
    }

    std::string header_label = "All Plugins (" + std::to_string(vxe::get_current_context()->IO.ep.size()) + ")";
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
            header_width,
            header_label.c_str(),
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      set_selected_category("all");
    }

    CherryGUI::PopStyleColor(4);

    CherryKit::Space(5.0f);

    CherryStyle::AddMarginX(6.0f);
    CherryKit::SeparatorText("All types");

    static std::string SearchPluginsString;
    CherryStyle::AddMarginX(6.0f);
    CherryNextComponent.SetProperty("size_x", input_width);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("description", "Search categories...");
    CherryNextComponent.SetProperty(
        "description_logo", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
    CherryNextComponent.SetProperty("description_logo_place", "r");
    CherryKit::InputString("", &SearchPluginsString);

    CherryGUI::BeginChild("####PluginsCategoryLists");
    for (const auto &[category, count] : all_categories) {
      if (!SearchPluginsString.empty()) {
        if (!has_common_subsequence(category, SearchPluginsString)) {
          continue;
        }
      }

      std::string label = category + " (" + std::to_string(count) + ")";
      CherryStyle::AddMarginX(6.0f);
      if (category == selected_category_) {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#787878"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#454545"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#565656"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
      } else {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
        CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
      }
      if (CherryGUI::ImageSizeButtonWithText(
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_white_brick.png")),
              header_width,
              label.c_str(),
              ImVec2(-FLT_MIN, 0.0f),
              ImVec2(0, 0),
              ImVec2(1, 1),
              -1,
              ImVec4(0, 0, 0, 0),
              ImVec4(1, 1, 1, 1))) {
        set_selected_category(category);
      }
      CherryGUI::PopStyleColor(4);
    }
    CherryGUI::EndChild();

    CherryGUI::PopStyleVar();

    /*for (const auto &child : childs_) {
      if (child.name == selected_child_name_) {
        // opt.hex_text_idle = "#FFFFFFFF";
      } else {
        // opt.hex_text_idle = "#A9A9A9FF";
      }
      std::string child_name;

      if (child.name.rfind("?loc:", 0) == 0) {
        std::string localeName = child.name.substr(5);
        child_name = Cherry::GetLocale(localeName) + "####" + localeName;
      } else {
        child_name = child.name;
      }

      CherryNextComponent.SetProperty("color_bg", "#00000000");
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("padding_x", "2");
      CherryNextComponent.SetProperty("padding_y", "2");
      CherryNextComponent.SetProperty("size_x", "20");
      CherryNextComponent.SetProperty("size_y", "20");
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);
      if (CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(),
                                     child.logo_path)
              .GetData("isClicked") == "true") {
        selected_child_name_ = child.name;
      }
    }
  */
    CherryGUI::EndChild();
    CherryGUI::PopStyleVar(4);
    CherryGUI::PopStyleColor(2);

    CherryGUI::SameLine();
    CherryGUI::BeginGroup();

    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);

    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

    if (CherryGUI::BeginChild(
            "ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
      CherryNextComponent.SetProperty("size_x", "240");
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("description", "Search plugin...");
      CherryNextComponent.SetProperty(
          "description_logo", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
      CherryNextComponent.SetProperty("description_logo_place", "r");
      CherryKit::InputString("", &plugins_search_);

      CherryNextProp("color", "#252525");
      CherryKit::Separator();

      CherryGUI::Spacing();

      if (vxe::get_current_context()->IO.ep.empty()) {
        CherryKit::TitleFour("No plugins founded.");
      }

      std::vector<Cherry::Component> plugins_blocks;
      if (selected_show_mode_ == PluginsUtilityShowModes::Thumbmails) {
        for (int i = 0; i < vxe::get_current_context()->IO.ep.size(); i++) {
          if (!vxe::get_current_context()->IO.ep[i]) {
            continue;
          }

          if (!selected_category_.empty() && selected_category_ != "all") {
            if (selected_category_ != vxe::get_current_context()->IO.ep[i]->group()) {
              continue;
            }
          }
          static std::string LatestChange;
          if (LatestChange != plugins_search_) {
            LatestChange = plugins_search_;
            selected_category_changed_ = true;
          }

          if (!plugins_search_.empty()) {
            if (!has_common_subsequence(vxe::get_current_context()->IO.ep[i]->name(), plugins_search_) &&
                !has_common_subsequence(vxe::get_current_context()->IO.ep[i]->proper_name(), plugins_search_) &&
                !has_common_subsequence(vxe::get_current_context()->IO.ep[i]->description(), plugins_search_)) {
              continue;
            }
          }

          CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
          auto item = PluginCard(
              vxe::get_current_context()->IO.ep[i],
              vxe::get_current_context()->IO.ep[i]->proper_name(),
              vxe::get_current_context()->IO.ep[i]->path(),
              vxe::get_current_context()->IO.ep[i]->name(),
              vxe::get_current_context()->IO.ep[i]->version(),
              false,
              vxe::get_current_context()->IO.ep[i]->logo_path(),
              IM_COL32(56, 56, 56, 150),
              IM_COL32(50, 50, 50, 255),
              Cherry::HexToImU32("#B1FF31FF"),
              100.0f,
              5.0f,
              [this](const std::shared_ptr<PluginInterface> &mod) { set_plugin_to_delete(mod); },
              &plugins_search_);

          plugins_blocks.push_back(item);
        }
        if (!selected_category_changed_) {
          CherryKit::GridSimple(250.0f, 250.0f, plugins_blocks);
        } else {
          selected_category_changed_ = false;
        }
      } else if (selected_show_mode_ == PluginsUtilityShowModes::List) {
        for (int i = 0; i < vxe::get_current_context()->IO.ep.size(); i++) {
          if (!vxe::get_current_context()->IO.ep[i]) {
            continue;
          }

          const auto &plu = vxe::get_current_context()->IO.ep[i];

          if (!selected_category_.empty() && selected_category_ != "all") {
            if (selected_category_ != plu->group()) {
              continue;
            }
          }

          if (!plugins_search_.empty()) {
            if (!has_common_subsequence(plu->name(), plugins_search_) &&
                !has_common_subsequence(plu->proper_name(), plugins_search_) &&
                !has_common_subsequence(plu->description(), plugins_search_)) {
              continue;
            }
          }

          render_plugin_list_row(plu);
          CherryKit::Space(6.0f);
        }
      }
    }
    CherryGUI::EndChild();

    CherryGUI::PopStyleVar(2);

    CherryGUI::EndGroup();
  }

  void PluginsUtility::render_left_menubar() {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);

    if (vxe::get_current_context()->IO.allow_net) {
      if (vxe::get_current_context()->disconnected) {
        CherryGUI::BeginDisabled();
      }

      CherryNextComponent.SetProperty("padding_y", "6.0f");
      if (CherryKit::ButtonImageText("Enter code", Cherry::GetPath("resources/imgs/icons/misc/icon_graynet.png"))
              .GetDataAs<bool>("isClicked")) {
        spawn_flash_link_window("prompt");
      }

      if (vxe::get_current_context()->disconnected) {
        CherryGUI::EndDisabled();
      }
    }

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText("Import", Cherry::GetPath("resources/imgs/icons/misc/icon_import.png"))
            .GetDataAs<bool>("isClicked")) {
      spawn_import_window();
    }

    CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#444444AA"));
    CherryGUI::Separator();
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 12));
    if (!vxe::get_current_context()->IO.allow_net) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");
      if (!vxe::get_current_context()->IO.allow_net) {
        CherryNextComponent.SetProperty("color_border", "#00000000");
        CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
        CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
        CherryNextComponent.SetProperty("padding_y", "6.0f");
        CherryNextComponent.SetProperty("padding_x", "10.0f");
        if (CherryKit::ButtonImageText("Allow net features", Cherry::GetPath("resources/imgs/icons/misc/icon_graynet.png"))
                .GetDataAs<bool>("isClicked")) {
          trigger_net_permission_modal_ = true;
        }
      }
    }

    if (vxe::get_current_context()->IO.allow_net) {
      if (vxe::get_current_context()->disconnected) {
        CherryGUI::BeginDisabled();
      }

      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_lightning.png"))
              .GetDataAs<bool>("isClicked")) {
        spawn_flash_link_window("flash");
      }

      if (vxe::get_current_context()->disconnected) {
        CherryGUI::EndDisabled();
      }
    }
  }

  void PluginsUtility::render_right_menubar() {
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    CherryNextComponent.SetProperty("disable_callback", "true");
    if (CherryKit::ButtonImageTextDropdown("Settings", Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
            .GetDataAs<bool>("isClicked")) {
      ImVec2 mousePos = CherryGUI::GetMousePos();
      ImVec2 displaySize = CherryGUI::GetIO().DisplaySize;
      ImVec2 popupSize(350, 100);

      if (mousePos.x + popupSize.x > displaySize.x) {
        mousePos.x -= popupSize.x;
      }
      if (mousePos.y + popupSize.y > displaySize.y) {
        mousePos.y -= popupSize.y;
      }

      CherryGUI::SetNextWindowSize(popupSize, ImGuiCond_Appearing);
      CherryGUI::SetNextWindowPos(mousePos, ImGuiCond_Appearing);
      CherryGUI::OpenPopup("ViewMenuPopup");
    }
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);               // TODO : Props
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);      // TODO : Props
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);  // TODO : Props
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);        // TODO : Props

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);
    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    if (CherryGUI::BeginPopup("ViewMenuPopup")) {
      CherryKit::SeparatorText("View mode");

      int default_index = 0;

      if (selected_show_mode_ == PluginsUtilityShowModes::Thumbmails) {
        default_index = 0;
      } else if (selected_show_mode_ == PluginsUtilityShowModes::List) {
        default_index = 1;
      }

      switch (CherryNextComponent.SetProperty("size_x", 150.0f);
              CherryKit::ComboImageText(
                  "",
                  {
                      { "Thumbnails", Cherry::GetPath("resources/imgs/icons/misc/icon_thumbnails.png") },
                      { "List", Cherry::GetPath("resources/imgs/icons/misc/icon_lines.png") },
                  },
                  default_index)
                  .GetPropertyAs<int>("selected")) {
        case 0: {
          selected_show_mode_ = PluginsUtilityShowModes::Thumbmails;
          break;
        }
        case 1: {
          selected_show_mode_ = PluginsUtilityShowModes::List;
          break;
        }
        default: {
          selected_show_mode_ = PluginsUtilityShowModes::Thumbmails;
          break;
        }
      }

      CherryGUI::EndPopup();
    }

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor(2);
  }

  void PluginsUtility::spawn_import_window() {
    import_windows_counter_++;
    Cherry::ApplicationSpecification spec;

    std::string name = "?loc:loc.window_names.import" + std::to_string(import_windows_counter_);
    auto new_win = vxe::ImportPluginWindow::create(name);
    new_win->get_app_window()->SetVisibility(true);

    std::string label = "Import plugins";
    spec.Name = label;
    spec.MinHeight = 300;
    spec.MinWidth = 175;
    spec.Height = 700;
    spec.DisableLogo = true;
    spec.DisableResize = true;
    spec.Width = 500;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = new_win->get_app_window()->m_Name;
    spec.FramebarCallback = []() { };
    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, new_win]() { Cherry::DeleteAppWindow(new_win->get_app_window()); };

    spec.MenubarCallback = []() { };
    spec.WindowSaves = false;
    new_win->get_app_window()->AttachOnNewWindow(spec);
    Cherry::AddAppWindow(new_win->get_app_window());
  }

  void PluginsUtility::spawn_flash_link_window(const std::string &mode) {
    flash_link_windows_counter_++;
    Cherry::ApplicationSpecification spec;

    std::string name = "?loc:loc.window_names.flash_link" + std::to_string(flash_link_windows_counter_);
    auto new_win = vxe::FlashLinkPluginWindow::create(name, mode);
    new_win->get_app_window()->SetVisibility(true);

    std::string label = "Use flash link";
    spec.Name = label;
    spec.MinHeight = 300;
    spec.MinWidth = 175;
    spec.Height = 700;
    spec.DisableLogo = true;
    spec.DisableResize = true;
    spec.Width = 500;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = new_win->get_app_window()->m_Name;
    spec.FramebarCallback = []() { };
    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, new_win]() { Cherry::DeleteAppWindow(new_win->get_app_window()); };

    spec.MenubarCallback = []() { };
    spec.WindowSaves = false;
    new_win->get_app_window()->AttachOnNewWindow(spec);
    Cherry::AddAppWindow(new_win->get_app_window());
  }
}  // namespace vxe
