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
}

void ProjectViewer::OnImGuiRender()
{
        static ImTextureID listIcon = this->m_ListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if (ImGui::Begin("Project Viewer", &listIcon, &this->opened, ImGuiWindowFlags_MenuBar))
    this->menubar();

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

            if (ImGui::BeginMenuBar())
            {   
                ImGui::Checkbox("Collapse all", &this->CollapseAll);
                ImGui::Separator();
                if (ImGui::BeginMenu("Build"))
                {
                    if (ImGui::MenuItem("Build/Rebuild single parts"))
                    {
                    }
                    if (ImGui::MenuItem("Global build"))
                    {
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            
    }