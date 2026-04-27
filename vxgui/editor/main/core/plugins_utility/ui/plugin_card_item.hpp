//
//  plugin_card_item.hpp
//  Plugin card UI component, for rendering plugins and show quick
//  actions.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../../../../lib/cherry/cherry.hpp"
#include "../../../../../../vxcore/include/plugins/delete.h"
#include "../../../../../../vxcore/include/plugins/load.h"
#include "../../../../../../vxcore/include/vortex.h"
#include "../../../../../../vxcore/include/vortex_internals.h"
#include "../../../instances/modules_details/modules_details.hpp"

namespace vxe {
  static void DrawPluginCardHighlightedText(
      ImDrawList *drawList,
      ImVec2 textPos,
      const char *text,
      const char *search,
      ImU32 highlightColor,
      ImU32 textColor,
      ImU32 highlightTextColor) {
    if (!text || !search || !*search) {
      drawList->AddText(textPos, textColor, text);
      return;
    }

    const char *start = text;
    const char *found = strstr(start, search);
    while (found) {
      if (found > start) {
        std::string preText(start, found);
        drawList->AddText(textPos, textColor, preText.c_str());
        textPos.x += CherryGUI::CalcTextSize(preText.c_str()).x;
      }

      ImVec2 highlightPos = textPos;
      ImVec2 highlightSize = CherryGUI::CalcTextSize(search);
      drawList->AddRectFilled(
          highlightPos, ImVec2(highlightPos.x + highlightSize.x, highlightPos.y + highlightSize.y), highlightColor);
      drawList->AddText(textPos, highlightTextColor, search);
      textPos.x += highlightSize.x;

      start = found + strlen(search);
      found = strstr(start, search);
    }

    if (*start) {
      drawList->AddText(textPos, textColor, start);
    }
  }

  static void DrawPluginCardDescription(
      ImDrawList *drawList,
      ImVec2 textPos,
      const char *text,
      const char *search,
      ImU32 highlightColor,
      ImU32 textColor,
      ImU32 highlightTextColor,
      float oldfontsize) {
    CherryGUI::GetFont()->Scale = 0.7f;
    CherryGUI::PushFont(CherryGUI::GetFont());

    if (!text || !search || !*search) {
      drawList->AddText(textPos, textColor, text);
      return;
    }

    const char *start = text;
    const char *found = strstr(start, search);
    while (found) {
      if (found > start) {
        std::string preText(start, found);
        drawList->AddText(textPos, textColor, preText.c_str());
        textPos.x += CherryGUI::CalcTextSize(preText.c_str()).x;
      }

      ImVec2 highlightPos = textPos;
      ImVec2 highlightSize = CherryGUI::CalcTextSize(search);
      drawList->AddRectFilled(
          highlightPos, ImVec2(highlightPos.x + highlightSize.x, highlightPos.y + highlightSize.y), highlightColor);
      drawList->AddText(textPos, highlightTextColor, search);
      textPos.x += highlightSize.x;

      start = found + strlen(search);
      found = strstr(start, search);
    }

    if (*start) {
      drawList->AddText(textPos, textColor, start);
    }
    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();
  }

  class PluginCardItem : public Cherry::Component {
   public:
    PluginCardItem(
        const Cherry::Identifier &identifier,
        const std::shared_ptr<PluginInterface> &plugin,
        const std::string &name,
        const std::string &path,
        const std::string &description,
        const std::string &size,
        bool selected,
        const std::string &logo,
        ImU32 bgColor,
        ImU32 borderColor,
        ImU32 lineColor,
        float maxTextWidth,
        float borderRadius,
        std::function<void(const std::shared_ptr<PluginInterface> &mod)> deletion_callback,
        std::string *search_ptr)

        : Cherry::Component(identifier),
          m_plugin(plugin),
          m_name(name),
          m_path(path),
          m_description(description),
          m_size(size),
          m_selected(selected),
          m_logo(logo),
          m_bgColor(bgColor),
          m_borderColor(borderColor),
          m_lineColor(lineColor),
          m_maxTextWidth(maxTextWidth),
          m_borderRadius(borderRadius),
          plugin_deletion_callback_(deletion_callback),
          search_ptr_(search_ptr) {
    }

    void Render() override {
      bool pressed = false;

      float logoSize = 60.0f;
      float extraHeight = 80.0f;
      float padding = 10.0f;
      float separatorHeight = 2.0f;
      float textOffsetY = 5.0f;
      float versionBoxWidth = 10.0f;
      float versionBoxHeight = 20.0f;
      float thumbnailIconOffsetY = 30.0f;

      float oldfontsize = CherryGUI::GetFont()->Scale;
      ImFont *oldFont = CherryGUI::GetFont();

      ImVec2 squareSize(logoSize, logoSize);

      const char *originalText = m_name.c_str();
      std::string truncatedText = m_name;

      if (CherryGUI::CalcTextSize(originalText).x > m_maxTextWidth) {
        size_t len = m_name.size();

        size_t firstPart = std::min<size_t>(20, len);
        truncatedText = m_name.substr(0, firstPart);

        if (CherryGUI::CalcTextSize(truncatedText.c_str()).x > m_maxTextWidth) {
          size_t firstLine = std::min<size_t>(10, len);
          size_t secondLine = (len > 10) ? std::min<size_t>(10, len - 10) : 0;

          truncatedText = m_name.substr(0, firstLine);
          if (secondLine > 0) {
            truncatedText += "\n" + m_name.substr(10, secondLine);
          }
        }
      } else {
        truncatedText = m_name + "\n";
      }

      std::string truncatedDesc = m_plugin->m_description;

      if (truncatedDesc.length() > 100) {
        truncatedDesc = truncatedDesc.substr(0, 97) + "...";
      }
      const char *originalDesc = truncatedDesc.c_str();

      if (CherryGUI::CalcTextSize(originalDesc).x > m_maxTextWidth) {
        size_t len = m_plugin->m_description.size();

        size_t firstPart = std::min<size_t>(90, len);
        truncatedDesc = m_plugin->m_description.substr(0, firstPart);

        if (CherryGUI::CalcTextSize(truncatedDesc.c_str()).x > m_maxTextWidth) {
          size_t firstLine = std::min<size_t>(55, len);
          size_t secondLine = (len > 55) ? len - 55 : 0;

          truncatedDesc = m_plugin->m_description.substr(0, firstLine);
          if (secondLine > 0) {
            truncatedDesc += "\n" + m_plugin->m_description.substr(55, secondLine);
          }
        }
      } else {
        truncatedDesc = m_plugin->m_description + "\n";
      }

      ImVec2 fixedSize(m_maxTextWidth + padding * 2 + 150.0f, logoSize + extraHeight + padding * 2);

      ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

      if (CherryGUI::IsItemHovered()) {
        CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
      }

      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
      ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

      CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
      CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

      CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

      static bool open_deletion_modal = false;

      static bool delete_single_file = false;
      static std::string delete_single_file_path = "";

      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor(2);

      ImDrawList *drawList = CherryGUI::GetWindowDrawList();

      drawList->AddRectFilled(
          cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), m_bgColor, m_borderRadius);
      drawList->AddRectFilled(
          cursorPos,
          ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + thumbnailIconOffsetY + squareSize.y),
          IM_COL32(26, 26, 26, 255),
          m_borderRadius,
          ImDrawFlags_RoundCornersTop);
      drawList->AddRect(
          cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), m_borderColor, m_borderRadius, 0, 1.0f);

      ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2, cursorPos.y + padding);

      ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY - 25 + textOffsetY);
      CherryGUI::SetCursorScreenPos(sizePos);

      ImTextureID logotexture = Cherry::Application::GetCurrentRenderedWindow()->get_texture(m_logo);
      drawList->AddImage(logotexture, logoPos, ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

      CherryGUI::GetFont()->Scale = 0.7f;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
      CherryGUI::PushItemWidth(m_maxTextWidth);
      CherryGUI::TextWrapped(m_size.c_str());
      CherryGUI::PopItemWidth();
      CherryGUI::PopStyleColor();

      CherryGUI::GetFont()->Scale = oldfontsize;
      CherryGUI::PopFont();

      ImVec2 lineStart = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
      ImVec2 lineEnd =
          ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
      drawList->AddLine(lineStart, lineEnd, m_lineColor, separatorHeight);

      CherryGUI::GetFont()->Scale = 0.9f;
      CherryGUI::PushFont(CherryGUI::GetFont());

      ImVec2 textPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
      CherryGUI::SetCursorScreenPos(textPos);
      CherryGUI::PushItemWidth(m_maxTextWidth);
      ImU32 textColor = IM_COL32(255, 255, 255, 255);
      ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
      ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
      if (search_ptr_) {
        DrawPluginCardHighlightedText(
            drawList, textPos, truncatedText.c_str(), search_ptr_->c_str(), highlightColor, textColor, highlightTextColor);
      }
      CherryGUI::PopItemWidth();

      CherryGUI::GetFont()->Scale = oldfontsize;
      CherryGUI::PopFont();

      ImVec2 labelPos =
          ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y - 20 + thumbnailIconOffsetY + 35 + textOffsetY);
      CherryGUI::SetCursorScreenPos(labelPos);

      CherryGUI::GetFont()->Scale = 0.7f;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
      CherryGUI::PushItemWidth(m_maxTextWidth);
      CherryGUI::TextWrapped(m_description.c_str());
      CherryGUI::PopItemWidth();
      CherryGUI::PopStyleColor();

      ImVec2 descriptionPos =
          ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + 35 + textOffsetY);
      CherryGUI::SetCursorScreenPos(descriptionPos);

      CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
      CherryGUI::PushItemWidth(fixedSize.x);

      if (search_ptr_) {
        DrawPluginCardDescription(
            drawList,
            descriptionPos,
            truncatedDesc.c_str(),
            search_ptr_->c_str(),
            highlightColor,
            Cherry::HexToImU32("#8A8A8AFF"),
            highlightTextColor,
            oldfontsize);
      }

      CherryGUI::PopItemWidth();
      CherryGUI::PopStyleColor();

      CherryGUI::GetFont()->Scale = oldfontsize;
      CherryGUI::PopFont();

      float buttonWidth = 20.0f;
      float buttonHeight = 20.0f;
      float buttonSpacing = 10.0f;

      ImVec2 firstButtonPos = ImVec2(cursorPos.x + fixedSize.x - padding - buttonWidth - 10, cursorPos.y + padding);
      CherryGUI::SetCursorScreenPos(firstButtonPos);

      if (m_plugin->m_state == "failed") {
        if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_retry.png"))
                .GetDataAs<bool>("isClicked")) {
          m_plugin->start();
        }

      } else if (m_plugin->m_state == "unknow" || m_plugin->m_state == "stopped") {
        if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_start.png"))
                .GetDataAs<bool>("isClicked")) {
          m_plugin->start();
        }

      } else {
        if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_stop.png"))
                .GetDataAs<bool>("isClicked")) {
          m_plugin->stop();
        }
      }

      ImVec2 secondButtonPos = ImVec2(firstButtonPos.x - buttonWidth - buttonSpacing, firstButtonPos.y);
      CherryGUI::SetCursorScreenPos(secondButtonPos);

      if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png")).GetDataAs<bool>("isClicked")) {
        m_plugin->stop();
        if (plugin_deletion_callback_) {
          plugin_deletion_callback_(m_plugin);
        }
      }

      ImVec2 thirdButtonPos = ImVec2(secondButtonPos.x - buttonWidth - buttonSpacing, secondButtonPos.y);
      CherryGUI::SetCursorScreenPos(thirdButtonPos);
      if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
              .GetDataAs<bool>("isClicked")) {
      }

      /*

         if (ctx->IO.ep[i]->m_state == "failed")
          {
              if (CherryGUI::ImageButtonWithText(listIcon, "Retry to launch",
         ImVec2(this->m_RefreshIcon->GetWidth(),
         this->m_RefreshIcon->GetHeight())))
              {
                  ctx->IO.ep[i]->start();
              }
              CherryGUI::SameLine();
              if (CherryGUI::ImageButtonWithText(trashIcon, "Delete",
         ImVec2(this->m_RefreshIcon->GetWidth(),
         this->m_RefreshIcon->GetHeight())))
              {
                  // Behavior
              }
              CherryGUI::SameLine();
              if (CherryGUI::ImageButtonWithText(logIcon, "Details",
         ImVec2(this->m_RefreshIcon->GetWidth(),
         this->m_RefreshIcon->GetHeight())))
              {
                  // Behavior
                  std::shared_ptr<ModuleDetails> instance =
         std::make_shared<ModuleDetails>(this->ctx, ctx->IO.ep[i]);
                  this->factory->SpawnInstance(instance);
              }
          }
          if (ctx->IO.ep[i]->m_state == "unknow" || ctx->IO.ep[i]->m_state ==
         "stopped")
          {
              if (CherryGUI::ImageButtonWithText(startIcon, "Launch",
         ImVec2(this->m_RefreshIcon->GetWidth(),
         this->m_RefreshIcon->GetHeight())))
              {
                  ctx->IO.ep[i]->start();
              }
              CherryGUI::SameLine();
              if (CherryGUI::ImageButtonWithText(trashIcon, "Delete",
         ImVec2(this->m_RefreshIcon->GetWidth(),
         this->m_RefreshIcon->GetHeight())))
              {
                  // Behavior
              }
              CherryGUI::SameLine();
              if (CherryGUI::ImageButtonWithText(logIcon, "Details",
         ImVec2(this->m_RefreshIcon->GetWidth(),
         this->m_RefreshIcon->GetHeight())))
              {
                  // Behavior
                  std::shared_ptr<ModuleDetails> instance =
         std::make_shared<ModuleDetails>(this->ctx, ctx->IO.ep[i]);
                  this->factory->SpawnInstance(instance);
              }
          }
    */

      float windowVisibleX2 = CherryGUI::GetWindowPos().x + CherryGUI::GetWindowContentRegionMax().x;
      if (cursorPos.x + fixedSize.x < windowVisibleX2)
        CherryGUI::SameLine();

      CherryGUI::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

      CherryGUI::GetFont()->Scale = oldfontsize;

      // return pressed;
    }

   private:
    std::function<void(const std::shared_ptr<PluginInterface> &mod)> plugin_deletion_callback_;
    std::shared_ptr<PluginInterface> m_plugin;
    std::string m_name;
    std::string m_path;
    std::string m_description;
    std::string m_size;
    bool m_selected;
    std::string m_logo;
    ImU32 m_bgColor;
    ImU32 m_borderColor;
    ImU32 m_lineColor;
    float m_maxTextWidth;
    float m_borderRadius;

    std::string *search_ptr_;
  };

  inline Cherry::Component &PluginCardComponent(
      const Cherry::Identifier &identifier,
      const std::shared_ptr<PluginInterface> &plugin,
      const std::string &name,
      const std::string &path,
      const std::string &description,
      const std::string &size,
      bool selected,
      const std::string &logo,
      ImU32 bgColor,
      ImU32 borderColor,
      ImU32 lineColor,
      float maxTextWidth,
      float borderRadius,
      std::function<void(const std::shared_ptr<PluginInterface> &mod)> deletion_callback,
      std::string *search_ptr) {
    return CherryApp.PushComponent<PluginCardItem>(
        identifier,
        plugin,
        name,
        path,
        description,
        size,
        selected,
        logo,
        bgColor,
        borderColor,
        lineColor,
        maxTextWidth,
        borderRadius,
        deletion_callback,
        search_ptr);
  }

  inline Cherry::Component &PluginCard(
      const std::shared_ptr<PluginInterface> &plugin,
      const std::string &name,
      const std::string &path,
      const std::string &description,
      const std::string &size,
      bool selected,
      const std::string &logo,
      ImU32 bgColor,
      ImU32 borderColor,
      ImU32 lineColor,
      float maxTextWidth,
      float borderRadius,
      std::function<void(const std::shared_ptr<PluginInterface> &mod)> deletion_callback,
      std::string *search_ptr) {
    return PluginCardComponent(
        Cherry::Application::GenerateUniqueID(
            plugin,
            name,
            path,
            description,
            size,
            selected,
            logo,
            bgColor,
            borderColor,
            lineColor,
            maxTextWidth,
            borderRadius,
            deletion_callback,
            search_ptr,
            "PluginCardComponent"),
        plugin,
        name,
        path,
        description,
        size,
        selected,
        logo,
        bgColor,
        borderColor,
        lineColor,
        maxTextWidth,
        borderRadius,
        deletion_callback,
        search_ptr);
  }

}  // namespace vxe