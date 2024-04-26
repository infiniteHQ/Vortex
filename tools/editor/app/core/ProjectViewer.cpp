#include "ProjectViewer.hpp"

#include <iostream>

static int item_current = 0;

ProjectViewer::ProjectViewer(VxContext *_ctx, InstanceFactory *_factory)
{
    this->ctx = _ctx;
    this->factory = _factory;
    this->refreshContents();

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
}

void ProjectViewer::OnImGuiRender()
{
    static ImTextureID listIcon = this->m_ListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    this->refreshContents();
    if (ImGui::Begin("Project Viewer", &listIcon, &this->opened, ImGuiWindowFlags_MenuBar))
    {
        this->menubar();
    }

    float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.3;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Project contents of : ");
    ImGui::SameLine();
    //ImGui::Text(this->ctx->name.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    ImGui::Separator();

   
    if(item_current == 0){
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
    }
    if(item_current == 1){
        // Behavior
    }


    ImGui::End();
}

void ProjectViewer::menubar()
{

    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::BeginMenuBar())
    {
        const char *items[] = {"Treev iew", "Cards view"};

        ImGui::Combo("View", &item_current, items, IM_ARRAYSIZE(items));

        ImGui::Separator();

        if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
            this->refreshContents();
        }
        if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
        {
            ImGui::OpenPopup("CreationMenu");
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Filters"))
        {

            if (ImGui::MenuItem("Build/Rebuild single parts"))
            {
        // Behavior
            }
            if (ImGui::MenuItem("Global build"))
            {
        // Behavior
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}