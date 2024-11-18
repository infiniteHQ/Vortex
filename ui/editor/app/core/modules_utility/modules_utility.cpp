#include "./modules_utility.hpp"
#include "../../instances/modules_details/modules_details.hpp"

using namespace Cherry;

static int item_current = 0;
static bool open_ADDMODULE = false;
static std::string s_Parent;

static int mb_selected = 0;
static std::vector<std::string> mb_syslabels;

namespace VortexEditor
{
    void ShowSystemModules(const std::string &filter = "none")
    {
        for (int row = 0; row < m_VortexRegisteredVersions.size(); row++)
        {
            if (areStringsSimilar(m_VortexRegisteredVersions[row]->m_VersionName, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
            {
                VersionButton(m_VortexRegisteredVersions[row]->m_VersionName, 300, 100, m_VortexRegisteredVersions[row]->m_Version);
            }
        }
    }

    void ModulesUtilityAppWindow::ShowModules()
    {
        /*for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.em.size(); i++)
                {
                    resetValue(VortexMaker::GetCurrentContext()->IO.em[i]->m_group);
                }*/

        for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.em.size(); i++)
        {
            ModuleCard(VortexMaker::GetCurrentContext()->IO.em[i], VortexMaker::GetCurrentContext()->IO.em[i]->m_proper_name, VortexMaker::GetCurrentContext()->IO.em[i]->m_path, VortexMaker::GetCurrentContext()->IO.em[i]->m_name, VortexMaker::GetCurrentContext()->IO.em[i]->m_version, false, VortexMaker::GetCurrentContext()->IO.em[i]->m_logo_path, IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255), Cherry::HexToImU32("#B1FF31FF"), 100.0f, 5.0f);
        }
    }

    bool ModulesUtilityAppWindow::ModuleCard(const std::shared_ptr<ModuleInterface> &module, const std::string &name, const std::string &path, const std::string &description, const std::string &size, bool selected, const std::string &logo, ImU32 bgColor = IM_COL32(100, 100, 100, 255), ImU32 borderColor = IM_COL32(150, 150, 150, 255), ImU32 lineColor = IM_COL32(255, 255, 0, 255), float maxTextWidth = 100.0f, float borderRadius = 5.0f)
    {
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

        if (selected)
        {
            bgColor = IM_COL32(80, 80, 240, 255);
            borderColor = IM_COL32(150, 150, 255, 255);
        }

        ImVec2 squareSize(logoSize, logoSize);

        const char *originalText = name.c_str();
        std::string truncatedText = name;

        if (ImGui::CalcTextSize(originalText).x > maxTextWidth)
        {
            truncatedText = name.substr(0, 20);
            if (ImGui::CalcTextSize(truncatedText.c_str()).x > maxTextWidth)
            {
                truncatedText = name.substr(0, 10) + "\n" + name.substr(10, 10);
            }
        }
        else
        {
            truncatedText = name + "\n";
        }

        ImVec2 fixedSize(maxTextWidth + padding * 2 + 150.0f, logoSize + extraHeight + padding * 2);

        ImVec2 cursorPos = ImGui::GetCursorScreenPos();

        if (ImGui::IsItemHovered())
        {
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

        drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), bgColor, borderRadius);
        drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + thumbnailIconOffsetY + squareSize.y), IM_COL32(26, 26, 26, 255), borderRadius, ImDrawFlags_RoundCornersTop);
        drawList->AddRect(cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), borderColor, borderRadius, 0, 1.0f);

        ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2, cursorPos.y + padding);

        ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY - 25 + textOffsetY);
        ImGui::SetCursorScreenPos(sizePos);

        ImTextureID logotexture = Application::GetCurrentRenderedWindow()->get_texture(logo);
        drawList->AddImage(logotexture, logoPos, ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

        ImGui::GetFont()->Scale = 0.7f;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushItemWidth(maxTextWidth);
        ImGui::TextWrapped(size.c_str());
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = oldfontsize;
        ImGui::PopFont();

        ImVec2 lineStart = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
        ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
        drawList->AddLine(lineStart, lineEnd, lineColor, separatorHeight);

        ImGui::GetFont()->Scale = 0.9f;
        ImGui::PushFont(ImGui::GetFont());

        ImVec2 textPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
        ImGui::SetCursorScreenPos(textPos);
        ImGui::PushItemWidth(maxTextWidth);
        ImU32 textColor = IM_COL32(255, 255, 255, 255);
        ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
        ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
        DrawHighlightedText(drawList, textPos, truncatedText.c_str(), ModulesSearch, highlightColor, textColor, highlightTextColor);

        ImGui::PopItemWidth();

        ImGui::GetFont()->Scale = oldfontsize;
        ImGui::PopFont();

        ImVec2 labelPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y - 20 + thumbnailIconOffsetY + 35 + textOffsetY);
        ImGui::SetCursorScreenPos(labelPos);

        ImGui::GetFont()->Scale = 0.7f;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushItemWidth(maxTextWidth);
        ImGui::TextWrapped(description.c_str());
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImVec2 descriptionPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + 35 + textOffsetY);
        ImGui::SetCursorScreenPos(descriptionPos);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushItemWidth(maxTextWidth);
        ImGui::TextWrapped(module->m_description.c_str());
        ImGui::PopItemWidth();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = oldfontsize;
        ImGui::PopFont();

        float buttonWidth = 20.0f;
        float buttonHeight = 20.0f;
        float buttonSpacing = 10.0f;

        ImVec2 firstButtonPos = ImVec2(cursorPos.x + fixedSize.x - padding - buttonWidth, cursorPos.y + padding);
        ImGui::SetCursorScreenPos(firstButtonPos);

        if (module->m_state == "failed")
        {
            {
                auto btn = std::make_shared<Cherry::ImageButtonSimple>("create_project_button", Cherry::GetPath("resources/imgs/icons/misc/icon_retry.png"));
                btn->SetScale(0.20f);
                btn->SetInternalMarginX(1.0f);
                btn->SetInternalMarginY(1.0f);
                btn->SetLogoSize(5, 5);
                btn->SetBorderColorIdle("#00000000");
                btn->SetBackgroundColorClicked("#00000000");

                if (btn->Render("StartModule"))
                {
                    module->Start();
                }
            }
        }
        else if (module->m_state == "unknow" || module->m_state == "stopped")
        {
            {
                auto btn = std::make_shared<Cherry::ImageButtonSimple>("create_project_button", Cherry::GetPath("resources/imgs/icons/misc/icon_start.png"));
                btn->SetScale(0.20f);
                btn->SetInternalMarginX(1.0f);
                btn->SetInternalMarginY(1.0f);
                btn->SetLogoSize(5, 5);
                btn->SetBorderColorIdle("#00000000");
                btn->SetBackgroundColorClicked("#00000000");

                if (btn->Render("StartModule2"))
                {
                    module->Start();
                }
            }
        }
        else
        {
        {
            auto btn = std::make_shared<Cherry::ImageButtonSimple>("four_button", Cherry::GetPath("resources/imgs/icons/misc/icon_stop.png"));
            btn->SetScale(0.20f);
            btn->SetInternalMarginX(1.0f);
            btn->SetInternalMarginY(1.0f);
            btn->SetLogoSize(5, 5);
            btn->SetBorderColorIdle("#00000000");
            btn->SetBackgroundColorClicked("#00000000");

            if (btn->Render("StopModule"))
            {
                module->Stop();
            }
        }
        }

        ImVec2 secondButtonPos = ImVec2(firstButtonPos.x - buttonWidth - buttonSpacing, firstButtonPos.y);
        ImGui::SetCursorScreenPos(secondButtonPos);

        {
            auto btn = std::make_shared<Cherry::ImageButtonSimple>("second_button", Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"));
            btn->SetScale(0.20f);
            btn->SetInternalMarginX(1.0f);
            btn->SetInternalMarginY(1.0f);
            btn->SetLogoSize(7, 7);
            btn->SetBorderColorIdle("#00000000");
            btn->SetBackgroundColorClicked("#00000000");

            if (btn->Render("edit"))
            {
                module->Stop();
                // TODO VortexMaker::DeleteProjectModule
            }
        }
        ImVec2 thirdButtonPos = ImVec2(secondButtonPos.x - buttonWidth - buttonSpacing, secondButtonPos.y);
        ImGui::SetCursorScreenPos(thirdButtonPos);

        {
            auto btn = std::make_shared<Cherry::ImageButtonSimple>("third_button", Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"));
            btn->SetScale(0.20f);
            btn->SetInternalMarginX(1.0f);
            btn->SetInternalMarginY(1.0f);
            btn->SetLogoSize(5, 5);
            btn->SetBorderColorIdle("#00000000");
            btn->SetBackgroundColorClicked("#00000000");

            if (btn->Render("OpenModulesDetails"))
            {
                std::string label = "Details of " + module->m_proper_name + "####" + path;
                std::shared_ptr<ModuleDetails> window = ModuleDetails::Create(label, module);
                Cherry::AddAppWindow(window->GetAppWindow());
            }
        }

        /*

           if (ctx->IO.em[i]->m_state == "failed")
            {
                if (ImGui::ImageButtonWithText(listIcon, "Retry to launch", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    ctx->IO.em[i]->Start();
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(logIcon, "Details", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                    std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
                    this->factory->SpawnInstance(instance);
                }
            }
            if (ctx->IO.em[i]->m_state == "unknow" || ctx->IO.em[i]->m_state == "stopped")
            {
                if (ImGui::ImageButtonWithText(startIcon, "Launch", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    ctx->IO.em[i]->Start();
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(logIcon, "Details", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                    std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
                    this->factory->SpawnInstance(instance);
                }
            }
*/

        float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        if (cursorPos.x + fixedSize.x < windowVisibleX2)
            ImGui::SameLine();

        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

        ImGui::GetFont()->Scale = oldfontsize;

        return pressed;
    }

    ModulesUtilityAppWindow::ModulesUtilityAppWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
        m_AppWindow->SetClosable(false);
        RegisterAvailableVersions();

        m_AppWindow->m_TabMenuCallback = []()
        {
            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
            ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
            ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
            if (ImGui::BeginMenu("Edit"))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                ImGui::Text("Main stuff");
                ImGui::PopStyleColor();

                ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                ImGui::Separator();
                ImGui::PopStyleColor();

                if (ImGui::MenuItem("Logs Utility", "Overview of all logs"))
                {
                }

                if (ImGui::MenuItem("Logs2 Utility", "Overview of all logs"))
                {
                }

                ImGui::EndMenu();
            }
        };

        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);

        m_SelectedChildName = "?loc:loc.window_names.welcome.overview";

        this->AddChild("Manage modules", "System modules", [this]()
                       { ShowModules(); });
        this->AddChild("Manage modules", "Import a module", [this]()
                       { ShowModules(); });

        std::string label = "All modules (" + std::to_string(VortexMaker::GetCurrentContext()->IO.em.size()) + ")";
        this->AddChild("All modules in project", label.c_str(), [this]()
                       { ShowSystemModules(); });

        this->AddChild("All modules in project (groups)", "Visual Development (21)", [this]()
                       { ShowModules(); });

        this->m_AppWindow->SetLeftMenubarCallback([]()
                                                  {
                        ImGui::Button("Import a module");
                        ImGui::Button("Search modules"); });
    }

    void ModulesUtilityAppWindow::AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child)
    {
        m_Childs.push_back(ModulesUtilityAppWindowChild(parent_name, child_name, child));
    }

    void ModulesUtilityAppWindow::RemoveChild(const std::string &child_name)
    {
    }

    std::function<void()> ModulesUtilityAppWindow::GetChild(const std::string &child_name)
    {
        for (auto &child : m_Childs)
        {
            if (child.m_ChildName == child_name)
            {
                return child.m_Callback;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Cherry::AppWindow> &ModulesUtilityAppWindow::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<ModulesUtilityAppWindow> ModulesUtilityAppWindow::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<ModulesUtilityAppWindow>(new ModulesUtilityAppWindow(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void ModulesUtilityAppWindow::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void ModulesUtilityAppWindow::Render()
    {
        static float leftPaneWidth = 300.0f;
        const float minPaneWidth = 50.0f;
        const float splitterWidth = 1.5f;
        static int selected;

        std::map<std::string, std::vector<ModulesUtilityAppWindowChild>> groupedByParent;
        for (const auto &child : m_Childs)
        {
            groupedByParent[child.m_Parent].push_back(child);
        }

        ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

        ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

        TitleThree("Manage Modules");
        for (const auto &[parent, children] : groupedByParent)
        {

            ImGui::GetFont()->Scale *= 0.8;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
            ImGui::Text(parent.c_str());
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
            ImGui::Separator();
            ImGui::PopStyleColor();

            ImGui::GetFont()->Scale = 0.84;
            ImGui::PopFont();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

            for (const auto &child : children)
            {
                if (child.m_ChildName == m_SelectedChildName)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                }

                if (TextButtonUnderline(child.m_ChildName.c_str()))
                {
                    m_SelectedChildName = child.m_ChildName;
                }

                ImGui::PopStyleColor();
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, HexToRGBA("#44444466"));
        ImGui::Button("splitter", ImVec2(splitterWidth, -1));
        ImGui::PopStyleVar();

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (ImGui::IsItemActive())
        {
            float delta = ImGui::GetIO().MouseDelta.x;
            leftPaneWidth += delta;
            if (leftPaneWidth < minPaneWidth)
                leftPaneWidth = minPaneWidth;
        }

        ImGui::SameLine();
        ImGui::BeginGroup();

        if (!m_SelectedChildName.empty())
        {
            std::function<void()> pannel_render = GetChild(m_SelectedChildName);
            if (pannel_render)
            {
                pannel_render();
            }
        }

        ImGui::EndGroup();
    }
}
