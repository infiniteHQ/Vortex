#pragma once

#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#include "../utils.hpp"

#ifndef MODULES_UTILITY_WINDOW_H
#define MODULES_UTILITY_WINDOW_H

#include "../../../../../lib/cherry/cherry.hpp"

namespace VortexEditor {

static std::string m_ModulesSearch;

class ModuleCardItem : public Cherry::Component {
public:
  ModuleCardItem(const Cherry::Identifier &identifier,
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

    float oldfontsize = ImGui::GetFont()->Scale;
    ImFont *oldFont = ImGui::GetFont();

    if (selected) {
      m_bgColor = IM_COL32(80, 80, 240, 255);
      m_borderColor = IM_COL32(150, 150, 255, 255);
    }

    ImVec2 squareSize(logoSize, logoSize);

    const char *originalText = m_name.c_str();
    std::string truncatedText = m_name;

    if (ImGui::CalcTextSize(originalText).x > m_maxTextWidth) {
      truncatedText = m_name.substr(0, 20);
      if (ImGui::CalcTextSize(truncatedText.c_str()).x > m_maxTextWidth) {
        truncatedText = m_name.substr(0, 10) + "\n" + m_name.substr(10, 10);
      }
    } else {
      truncatedText = m_name + "\n";
    }

    std::string truncatedDesc = m_module->m_description;
    if (truncatedDesc.length() > 100) {
      truncatedDesc = truncatedDesc.substr(0, 97) + "...";
    }
    const char *originalDesc = truncatedDesc.c_str();

    if (ImGui::CalcTextSize(originalDesc).x > m_maxTextWidth) {
      truncatedDesc = m_module->m_description.substr(0, 90);
      if (ImGui::CalcTextSize(truncatedDesc.c_str()).x > m_maxTextWidth) {
        truncatedDesc = m_module->m_description.substr(0, 55) + "\n" +
                        m_module->m_description.substr(55, 55);
      }
    } else {
      truncatedDesc = m_module->m_description + "\n";
    }

    ImVec2 fixedSize(m_maxTextWidth + padding * 2 + 150.0f,
                     logoSize + extraHeight + padding * 2);

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    if (ImGui::IsItemHovered()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    static bool open_deletion_modal = false;

    static bool delete_single_file = false;
    static std::string delete_single_file_path = "";

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    ImDrawList *drawList = ImGui::GetWindowDrawList();

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
    ImGui::SetCursorScreenPos(sizePos);

    ImTextureID logotexture =
        Cherry::Application::GetCurrentRenderedWindow()->get_texture(m_logo);
    drawList->AddImage(
        logotexture, logoPos,
        ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

    ImGui::GetFont()->Scale = 0.7f;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushItemWidth(m_maxTextWidth);
    ImGui::TextWrapped(m_size.c_str());
    ImGui::PopItemWidth();
    ImGui::PopStyleColor();

    ImGui::GetFont()->Scale = oldfontsize;
    ImGui::PopFont();

    ImVec2 lineStart =
        ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY +
                                separatorHeight);
    ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x,
                            cursorPos.y + squareSize.y + thumbnailIconOffsetY +
                                separatorHeight);
    drawList->AddLine(lineStart, lineEnd, m_lineColor, separatorHeight);

    ImGui::GetFont()->Scale = 0.9f;
    ImGui::PushFont(ImGui::GetFont());

    ImVec2 textPos =
        ImVec2(cursorPos.x + padding,
               cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
    ImGui::SetCursorScreenPos(textPos);
    ImGui::PushItemWidth(m_maxTextWidth);
    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
    ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
    DrawHighlightedText(drawList, textPos, truncatedText.c_str(),
                        m_ModulesSearch.c_str(), highlightColor, textColor,
                        highlightTextColor);

    ImGui::PopItemWidth();

    ImGui::GetFont()->Scale = oldfontsize;
    ImGui::PopFont();

    ImVec2 labelPos = ImVec2(cursorPos.x + padding,
                             cursorPos.y + squareSize.y - 20 +
                                 thumbnailIconOffsetY + 35 + textOffsetY);
    ImGui::SetCursorScreenPos(labelPos);

    ImGui::GetFont()->Scale = 0.7f;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushItemWidth(m_maxTextWidth);
    ImGui::TextWrapped(m_description.c_str());
    ImGui::PopItemWidth();
    ImGui::PopStyleColor();

    ImVec2 descriptionPos = ImVec2(cursorPos.x + padding,
                                   cursorPos.y + squareSize.y +
                                       thumbnailIconOffsetY + 35 + textOffsetY);
    ImGui::SetCursorScreenPos(descriptionPos);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushItemWidth(fixedSize.x);

    DrawDescription(drawList, descriptionPos, truncatedDesc.c_str(),
                    m_ModulesSearch.c_str(), highlightColor,
                    Cherry::HexToImU32("#8A8A8AFF"), highlightTextColor,
                    oldfontsize);
    ImGui::PopItemWidth();
    ImGui::PopStyleColor();

    ImGui::GetFont()->Scale = oldfontsize;
    ImGui::PopFont();

    float buttonWidth = 20.0f;
    float buttonHeight = 20.0f;
    float buttonSpacing = 10.0f;

    ImVec2 firstButtonPos =
        ImVec2(cursorPos.x + fixedSize.x - padding - buttonWidth - 10,
               cursorPos.y + padding);
    ImGui::SetCursorScreenPos(firstButtonPos);

    if (m_module->m_state == "failed") {

      if (CherryKit::ButtonImage(
              Cherry::GetPath("resources/imgs/icons/misc/icon_retry.png"))
              .GetPropertyAs<bool>("isClicked")) {
        m_module->Start();
      }

    } else if (m_module->m_state == "unknow" ||
               m_module->m_state == "stopped") {

      if (CherryKit::ButtonImage(
              Cherry::GetPath("resources/imgs/icons/misc/icon_start.png"))
              .GetPropertyAs<bool>("isClicked")) {
        m_module->Start();
      }

    } else {
      if (CherryKit::ButtonImage(
              Cherry::GetPath("resources/imgs/icons/misc/icon_stop.png"))
              .GetPropertyAs<bool>("isClicked")) {
        m_module->Stop();
      }
    }

    ImVec2 secondButtonPos = ImVec2(
        firstButtonPos.x - buttonWidth - buttonSpacing, firstButtonPos.y);
    ImGui::SetCursorScreenPos(secondButtonPos);

    if (CherryKit::ButtonImage(
            Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"))
            .GetPropertyAs<bool>("isClicked")) {
      m_module->Stop();
      // TODO VortexMaker::DeleteProjectModule
    }

    ImVec2 thirdButtonPos = ImVec2(
        secondButtonPos.x - buttonWidth - buttonSpacing, secondButtonPos.y);
    ImGui::SetCursorScreenPos(thirdButtonPos);
    if (CherryKit::ButtonImage(
            Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
            .GetPropertyAs<bool>("isClicked")) {
      std::string label =
          "Details of " + m_module->m_proper_name + "####" + m_path;
      /*std::shared_ptr<ModuleDetails> window =
          ModuleDetails::Create(label, module);
      Cherry::AddAppWindow(window->GetAppWindow());*/
    }

    /*

       if (ctx->IO.em[i]->m_state == "failed")
        {
            if (ImGui::ImageButtonWithText(listIcon, "Retry to launch",
       ImVec2(this->m_RefreshIcon->GetWidth(),
       this->m_RefreshIcon->GetHeight())))
            {
                ctx->IO.em[i]->Start();
            }
            ImGui::SameLine();
            if (ImGui::ImageButtonWithText(trashIcon, "Delete",
       ImVec2(this->m_RefreshIcon->GetWidth(),
       this->m_RefreshIcon->GetHeight())))
            {
                // Behavior
            }
            ImGui::SameLine();
            if (ImGui::ImageButtonWithText(logIcon, "Details",
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
            if (ImGui::ImageButtonWithText(startIcon, "Launch",
       ImVec2(this->m_RefreshIcon->GetWidth(),
       this->m_RefreshIcon->GetHeight())))
            {
                ctx->IO.em[i]->Start();
            }
            ImGui::SameLine();
            if (ImGui::ImageButtonWithText(trashIcon, "Delete",
       ImVec2(this->m_RefreshIcon->GetWidth(),
       this->m_RefreshIcon->GetHeight())))
            {
                // Behavior
            }
            ImGui::SameLine();
            if (ImGui::ImageButtonWithText(logIcon, "Details",
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
        ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (cursorPos.x + fixedSize.x < windowVisibleX2)
      ImGui::SameLine();

    ImGui::SetCursorScreenPos(
        ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

    ImGui::GetFont()->Scale = oldfontsize;

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
ModuleCardComponent(const Cherry::Identifier &identifier,
                    const std::shared_ptr<ModuleInterface> &module,
                    const std::string &name, const std::string &path,
                    const std::string &description, const std::string &size,
                    bool selected, const std::string &logo, ImU32 bgColor,
                    ImU32 borderColor, ImU32 lineColor, float maxTextWidth,
                    float borderRadius) {
  return CherryApp.PushComponent<ModuleCardItem>(
      identifier, module, name, path, description, size, selected, logo,
      bgColor, borderColor, lineColor, maxTextWidth, borderRadius);
}

inline Cherry::Component &
ModuleCard(const std::shared_ptr<ModuleInterface> &module,
           const std::string &name, const std::string &path,
           const std::string &description, const std::string &size,
           bool selected, const std::string &logo, ImU32 bgColor,
           ImU32 borderColor, ImU32 lineColor, float maxTextWidth,
           float borderRadius) {
  return ModuleCardComponent(
      Cherry::Application::GenerateUniqueID(
          module, name, path, description, size, selected, logo, bgColor,
          borderColor, lineColor, maxTextWidth, borderRadius,
          "ModuleCardComponent"),
      module, name, path, description, size, selected, logo, bgColor,
      borderColor, lineColor, maxTextWidth, borderRadius);
}

struct ModulesUtilityChild {
  std::function<void()> RenderCallback;
  std::string LogoPath;
  std::string Name;
  ModulesUtilityChild(
      const std::string &name,
      const std::function<void()> &rendercallback = []() {},
      const std::string &logopath = "undefined")
      : Name(name), RenderCallback(rendercallback), LogoPath(logopath) {};
};

class ModulesUtility : public std::enable_shared_from_this<ModulesUtility> {
public:
  ModulesUtility(const std::string &name);

  void AddChild(const ModulesUtilityChild &child);
  void RemoveChild(const std::string &child_name);
  ModulesUtilityChild *GetChild(const std::string &child_name);

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<ModulesUtility> Create(const std::string &name);
  void SetupRenderCallback();
  void Render();
  void ModulesRender();

  std::vector<ModulesUtilityChild> m_Childs;

  bool ModuleCardO(const std::shared_ptr<ModuleInterface> &module,
                   const std::string &name, const std::string &path,
                   const std::string &description, const std::string &size,
                   bool selected, const std::string &logo,
                   ImU32 bgColor = IM_COL32(100, 100, 100, 255),
                   ImU32 borderColor = IM_COL32(150, 150, 150, 255),
                   ImU32 lineColor = IM_COL32(255, 255, 0, 255),
                   float maxTextWidth = 100.0f, float borderRadius = 5.0f);

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
#endif // MODULEs_UTILITY_WINDOW_H