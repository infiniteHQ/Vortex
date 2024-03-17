#include "ProjectViewer.hpp"
#include "../../vendor/imgui/imgui.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"


#include <iostream>

ProjectViewer::ProjectViewer(VxContext *_ctx, InstanceFactory* _factory)
{
    this->ctx = _ctx;
    this->factory = _factory;
    this->refreshContents();

        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_list, icons::i_list_size, w, h);
            m_ListIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
}

void ProjectViewer::OnImGuiRender()
{
        static ImTextureID listIcon = this->m_ListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                    this->refreshContents();
        if (ImGui::Begin("Project Viewer", &listIcon, &this->opened, ImGuiWindowFlags_MenuBar))
    this->menubar();

            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.3;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Project contents of : ");
            ImGui::SameLine();
            ImGui::Text(this->ctx->name.c_str());

            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::Separator();



    static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    if (ImGui::BeginTable("3ways", 3, flags))
    {
        // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
        ImGui::TableHeadersRow();

    
        MyTreeNode::DisplayNode(&nodeInfos[0], nodeInfos, factory, ctx);

        ImGui::EndTable();
    }

    ImGui::End();
}

void ProjectViewer::menubar(){

            static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            if (ImGui::BeginMenuBar())
            {   
                ImGui::Checkbox("Collapse all", &this->CollapseAll);
                ImGui::Separator();

                if(ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight()))){
                    this->refreshContents();
                }
                if(ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight()))){
                    std::cout << "Add button clicked" << std::endl;
                    ImGui::OpenPopup("CreationMenu");
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Filters"))
                {

                    if (ImGui::MenuItem("Build/Rebuild single parts"))
                    {
                    }
                    if (ImGui::MenuItem("Global build"))
                    {
                    }
                    ImGui::EndMenu();
                }


        static bool open_edit_popup = false;
        static bool open_confirm_popup = false;
        if (open_confirm_popup)
        {
            if (ImGui::BeginPopupModal("ToolchainCreated"))
            {
                std::string text = "The toolchain \"" + this->latest_toolchain->name + "\" has been created !";
                ImGui::Text(text.c_str());

                // Call CreatePackage function from VxHost
                if (ImGui::Button("Open it directly", ImVec2(120, 0)))
                {

							std::shared_ptr<ToolchainInstance> instance = std::make_shared<ToolchainInstance>(ctx, this->latest_toolchain, factory);
							factory->SpawnInstance(instance);	

                    ImGui::CloseCurrentPopup();
                }

                // Call CreatePackage function from VxHost
                if (ImGui::Button("Close", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }


        if (open_confirm_popup)
            ImGui::OpenPopup("ToolchainCreated");

        if (open_edit_popup)
        {
            if (ImGui::BeginPopupModal("CreatePackage"))
            {
                // 3 text inputs
                static char label[128] = "";
                static char author[128] = "";
                // inputs widget
                ImGui::InputText("Name", label, IM_ARRAYSIZE(label));
                ImGui::InputText("Author", author, IM_ARRAYSIZE(author));

                // Call CreatePackage function from VxHost
                if (ImGui::Button("Create", ImVec2(120, 0)))
                {
                    std::string _label = label;
                    std::string _author = author;
                    
                    VortexMaker::CreateToolchain(_label, _author);

                    //open_confirm_popup = true;
                    open_edit_popup = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }


        if (open_edit_popup)
            ImGui::OpenPopup("CreatePackage");

        if (ImGui::BeginPopupModal("CreationMenu", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Get content");
            ImGui::Separator();

            ImGui::Text("Create Basic Component");
            ImGui::Separator();
            if (ImGui::ImageButtonWithText(addIcon, "Toolchains", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
            {
                open_edit_popup = true;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::ImageButtonWithText(addIcon, "Script", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
            {
            }
            if (ImGui::ImageButtonWithText(addIcon, "Tasklist", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
            {
            }

            ImGui::Text("Create Advanced Component");
            ImGui::Separator();
            if (ImGui::ImageButtonWithText(addIcon, "Chrooter", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
            {
            }

            // static int unused_i = 0;
            // ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

            static bool dont_ask_me_next_time = false;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            ImGui::PopStyleVar();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

                
                ImGui::EndMenuBar();
            }


            
    }