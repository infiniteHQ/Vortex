#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#include "../../instances/modules_details/modules_details.hpp"

#include "../utils.hpp"

#ifndef PLUGINS_UTILITY_WINDOW_H
#define PLUGINS_UTILITY_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexEditor {

static std::string m_PluginsSearch;

class PluginCardItem : public Cherry::Component {
public:
  PluginCardItem(const Cherry::Identifier &identifier,
                 const std::shared_ptr<ModuleInterface> &module,
                 const std::string &name, const std::string &path,
                 const std::string &description, const std::string &size,
                 bool selected, const std::string &logo, ImU32 bgColor,
                 ImU32 borderColor, ImU32 lineColor, float maxTextWidth,
                 float borderRadius)

      : Cherry::Component(identifier), m_module(module), m_name(name),
        m_path(path), m_description(description), m_size(size),
        m_selected(selected), m_logo(logo), m_bgColor(bgColor),
        m_borderColor(borderColor), m_lineColor(lineColor),
        m_maxTextWidth(maxTextWidth), m_borderRadius(borderRadius) {}
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

    if (selected) {
      m_bgColor = IM_COL32(80, 80, 240, 255);
      m_borderColor = IM_COL32(150, 150, 255, 255);
    }
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

    std::string truncatedDesc = m_module->m_description;

    if (truncatedDesc.length() > 100) {
      truncatedDesc = truncatedDesc.substr(0, 97) + "...";
    }
    const char *originalDesc = truncatedDesc.c_str();

    if (CherryGUI::CalcTextSize(originalDesc).x > m_maxTextWidth) {
      size_t len = m_module->m_description.size();

      size_t firstPart = std::min<size_t>(90, len);
      truncatedDesc = m_module->m_description.substr(0, firstPart);

      if (CherryGUI::CalcTextSize(truncatedDesc.c_str()).x > m_maxTextWidth) {
        size_t firstLine = std::min<size_t>(55, len);
        size_t secondLine = (len > 55) ? len - 55 : 0;

        truncatedDesc = m_module->m_description.substr(0, firstLine);
        if (secondLine > 0) {
          truncatedDesc +=
              "\n" + m_module->m_description.substr(55, secondLine);
        }
      }
    } else {
      truncatedDesc = m_module->m_description + "\n";
    }

    ImVec2 fixedSize(m_maxTextWidth + padding * 2 + 150.0f,
                     logoSize + extraHeight + padding * 2);

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
        cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y),
        m_bgColor, m_borderRadius);
    drawList->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + fixedSize.x,
               cursorPos.y + thumbnailIconOffsetY + squareSize.y),
        IM_COL32(26, 26, 26, 255), m_borderRadius, ImDrawFlags_RoundCornersTop);
    drawList->AddRect(
        cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y),
        m_borderColor, m_borderRadius, 0, 1.0f);

    ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2,
                            cursorPos.y + padding);

    ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y +
                                                       thumbnailIconOffsetY -
                                                       25 + textOffsetY);
    CherryGUI::SetCursorScreenPos(sizePos);

    ImTextureID logotexture =
        Cherry::Application::GetCurrentRenderedWindow()->get_texture(m_logo);
    drawList->AddImage(
        logotexture, logoPos,
        ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

    CherryGUI::GetFont()->Scale = 0.7f;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(m_maxTextWidth);
    CherryGUI::TextWrapped(m_size.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    ImVec2 lineStart =
        ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY +
                                separatorHeight);
    ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x,
                            cursorPos.y + squareSize.y + thumbnailIconOffsetY +
                                separatorHeight);
    drawList->AddLine(lineStart, lineEnd, m_lineColor, separatorHeight);

    CherryGUI::GetFont()->Scale = 0.9f;
    CherryGUI::PushFont(CherryGUI::GetFont());

    ImVec2 textPos =
        ImVec2(cursorPos.x + padding,
               cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
    CherryGUI::SetCursorScreenPos(textPos);
    CherryGUI::PushItemWidth(m_maxTextWidth);
    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
    ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
    DrawHighlightedText(drawList, textPos, truncatedText.c_str(),
                        m_PluginsSearch.c_str(), highlightColor, textColor,
                        highlightTextColor);

    CherryGUI::PopItemWidth();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    ImVec2 labelPos = ImVec2(cursorPos.x + padding,
                             cursorPos.y + squareSize.y - 20 +
                                 thumbnailIconOffsetY + 35 + textOffsetY);
    CherryGUI::SetCursorScreenPos(labelPos);

    CherryGUI::GetFont()->Scale = 0.7f;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(m_maxTextWidth);
    CherryGUI::TextWrapped(m_description.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();

    ImVec2 descriptionPos = ImVec2(cursorPos.x + padding,
                                   cursorPos.y + squareSize.y +
                                       thumbnailIconOffsetY + 35 + textOffsetY);
    CherryGUI::SetCursorScreenPos(descriptionPos);

    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(fixedSize.x);

    DrawDescription(drawList, descriptionPos, truncatedDesc.c_str(),
                    m_PluginsSearch.c_str(), highlightColor,
                    Cherry::HexToImU32("#8A8A8AFF"), highlightTextColor,
                    oldfontsize);
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    float buttonWidth = 20.0f;
    float buttonHeight = 20.0f;
    float buttonSpacing = 10.0f;

    ImVec2 firstButtonPos =
        ImVec2(cursorPos.x + fixedSize.x - padding - buttonWidth - 10,
               cursorPos.y + padding);
    CherryGUI::SetCursorScreenPos(firstButtonPos);

    if (m_module->m_state == "failed") {

      if (CherryKit::ButtonImage(
              Cherry::GetPath("resources/imgs/icons/misc/icon_retry.png"))
              .GetDataAs<bool>("isClicked")) {
        m_module->Start();
      }

    } else if (m_module->m_state == "unknow" ||
               m_module->m_state == "stopped") {

      if (CherryKit::ButtonImage(
              Cherry::GetPath("resources/imgs/icons/misc/icon_start.png"))
              .GetDataAs<bool>("isClicked")) {
        m_module->Start();
      }

    } else {
      if (CherryKit::ButtonImage(
              Cherry::GetPath("resources/imgs/icons/misc/icon_stop.png"))
              .GetDataAs<bool>("isClicked")) {
        m_module->Stop();
      }
    }

    ImVec2 secondButtonPos = ImVec2(
        firstButtonPos.x - buttonWidth - buttonSpacing, firstButtonPos.y);
    CherryGUI::SetCursorScreenPos(secondButtonPos);

    if (CherryKit::ButtonImage(
            Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"))
            .GetDataAs<bool>("isClicked")) {
      m_module->Stop();
      // TODO VortexMaker::DeleteProjectModule
    }

    ImVec2 thirdButtonPos = ImVec2(
        secondButtonPos.x - buttonWidth - buttonSpacing, secondButtonPos.y);
    CherryGUI::SetCursorScreenPos(thirdButtonPos);
    if (CherryKit::ButtonImage(
            Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
            .GetDataAs<bool>("isClicked")) {
      std::string label =
          "Details of " + m_module->m_proper_name + "####" + m_path;
      std::shared_ptr<ModuleDetails> window =
          ModuleDetails::Create(label, m_module);
      Cherry::AddAppWindow(window->GetAppWindow());
    }

    /*

       if (ctx->IO.em[i]->m_state == "failed")
        {
            if (CherryGUI::ImageButtonWithText(listIcon, "Retry to launch",
       ImVec2(this->m_RefreshIcon->GetWidth(),
       this->m_RefreshIcon->GetHeight())))
            {
                ctx->IO.em[i]->Start();
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
       std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
                this->factory->SpawnInstance(instance);
            }
        }
        if (ctx->IO.em[i]->m_state == "unknow" || ctx->IO.em[i]->m_state ==
       "stopped")
        {
            if (CherryGUI::ImageButtonWithText(startIcon, "Launch",
       ImVec2(this->m_RefreshIcon->GetWidth(),
       this->m_RefreshIcon->GetHeight())))
            {
                ctx->IO.em[i]->Start();
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
       std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
                this->factory->SpawnInstance(instance);
            }
        }
  */

    float windowVisibleX2 =
        CherryGUI::GetWindowPos().x + CherryGUI::GetWindowContentRegionMax().x;
    if (cursorPos.x + fixedSize.x < windowVisibleX2)
      CherryGUI::SameLine();

    CherryGUI::SetCursorScreenPos(
        ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

    CherryGUI::GetFont()->Scale = oldfontsize;

    // return pressed;
  }

private:
  std::shared_ptr<ModuleInterface> m_module;
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
};

inline Cherry::Component &
PluginCardComponent(const Cherry::Identifier &identifier,
                    const std::shared_ptr<ModuleInterface> &module,
                    const std::string &name, const std::string &path,
                    const std::string &description, const std::string &size,
                    bool selected, const std::string &logo, ImU32 bgColor,
                    ImU32 borderColor, ImU32 lineColor, float maxTextWidth,
                    float borderRadius) {
  return CherryApp.PushComponent<PluginCardItem>(
      identifier, module, name, path, description, size, selected, logo,
      bgColor, borderColor, lineColor, maxTextWidth, borderRadius);
}

inline Cherry::Component &
PluginCard(const std::shared_ptr<ModuleInterface> &module,
           const std::string &name, const std::string &path,
           const std::string &description, const std::string &size,
           bool selected, const std::string &logo, ImU32 bgColor,
           ImU32 borderColor, ImU32 lineColor, float maxTextWidth,
           float borderRadius) {
  return PluginCardComponent(
      Cherry::Application::GenerateUniqueID(
          module, name, path, description, size, selected, logo, bgColor,
          borderColor, lineColor, maxTextWidth, borderRadius,
          "PluginCardComponent"),
      module, name, path, description, size, selected, logo, bgColor,
      borderColor, lineColor, maxTextWidth, borderRadius);
}

struct PluginsUtilityChild {
  std::function<void()> RenderCallback;
  std::string LogoPath;
  std::string Name;
  PluginsUtilityChild(
      const std::string &name,
      const std::function<void()> &rendercallback = []() {},
      const std::string &logopath = "undefined")
      : Name(name), RenderCallback(rendercallback), LogoPath(logopath) {};
};

class PluginsUtility : public std::enable_shared_from_this<PluginsUtility> {
public:
  PluginsUtility(const std::string &name);

  void AddChild(const PluginsUtilityChild &child);
  void RemoveChild(const std::string &child_name);
  PluginsUtilityChild *GetChild(const std::string &child_name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<PluginsUtility> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();
  void PluginsRender();

  std::vector<PluginsUtilityChild> m_Childs;

  std::function<void()> m_CreateProjectCallback;
  std::function<void()> m_OpenProjectCallback;
  std::function<void()> m_SettingsCallback;
  std::function<void(const std::shared_ptr<EnvProject> &)> m_ProjectCallback;

  char ModulesSearch[512];
  std::vector<std::shared_ptr<EnvProject>> GetMostRecentProjects(
      const std::vector<std::shared_ptr<EnvProject>> &projects,
      size_t maxCount);
  std::vector<std::shared_ptr<EnvProject>> m_RecentProjects;
  std::string m_SelectedChildName;

  std::vector<std::string> vortexDists;
  std::string VortexEditorDist;
  std::string newDist;

  std::shared_ptr<Cherry::AppWindow> m_AppWindow;
  int selected;
  float leftPaneWidth = 290.0f;
};
} // namespace VortexEditor
#endif // PLUGINs_UTILITY_WINDOW_H