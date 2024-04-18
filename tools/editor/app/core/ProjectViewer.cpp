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

            {

            /*ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Hosts : ");
            int height = 0;
            if(this->ctx->IO.hosts.size() < 4)height = 220;
            if(this->ctx->IO.hosts.size() < 8 && this->ctx->IO.hosts.size() > 4)height = 440;
            if(this->ctx->IO.hosts.size() < 12 && this->ctx->IO.hosts.size() > 8)height = 880;
            if(this->ctx->IO.hosts.size() < 16 && this->ctx->IO.hosts.size() > 12)height = 1100;
            
            
            ImGui::BeginChild("Pans_VolatileTasks", ImVec2(-1, height), true);
            ImGui::Columns(4, NULL);

            int i = 0;

            for (int row = 0; row < this->ctx->IO.hosts.size(); row++)
            {
                std::string label = "packageView###" + std::to_string(row) + this->ctx->IO.hosts[row]->name + std::to_string(i);
                ImGuiID id = ImGui::GetID(label.c_str());
                ImGui::BeginChildFrame(id, ImVec2(0, 200), true);

                // Affichage des éléments individuels
                std::string ll = this->ctx->IO.hosts[row]->name;
                ImGui::Text(ll.c_str());

               

                if (ImGui::Button("Open", ImVec2(-1, 35)))
                {

                }

                ImGui::EndChildFrame();
                ImGui::NextColumn();
            }
            ImGui::EndChild();*/
            }




            {/*

            int i = 0;
            int height = 0;
            if(this->ctx->IO.toolchains.size() < 4)height = 220;
            if(this->ctx->IO.toolchains.size() < 7 && this->ctx->IO.toolchains.size() > 4)height = 440;
            if(this->ctx->IO.toolchains.size() < 11 && this->ctx->IO.toolchains.size() > 8)height = 880;
            if(this->ctx->IO.toolchains.size() < 15 && this->ctx->IO.toolchains.size() > 12)height = 1100;

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Toolchain : ");
            ImGui::BeginChild("Pans_VolatileTasksToolchains", ImVec2(-1, height), true);
            ImGui::Columns(4, NULL);

            for (int row = 0; row < this->ctx->IO.toolchains.size(); row++)
            {
                std::string label = "packageView###" + std::to_string(row) + this->ctx->IO.toolchains[row]->name + std::to_string(i);
                ImGuiID id = ImGui::GetID(label.c_str());
                ImGui::BeginChildFrame(id, ImVec2(0, 200), true);

                // Affichage des éléments individuels
                std::string ll = this->ctx->IO.toolchains[row]->name;
                ImGui::Text(ll.c_str());

               

				std::string toolchainName = "Open###" + this->ctx->IO.toolchains[row]->name + "Open";
				if(ImGui::Button(toolchainName.c_str(), ImVec2(-1, 35))){
							std::shared_ptr<ToolchainInstance> instance = std::make_shared<ToolchainInstance>(ctx, this->ctx->IO.toolchains[row], factory);
							factory->SpawnInstance(instance);	
						
					

				}
                ImGui::EndChildFrame();
                ImGui::NextColumn();
            }
            ImGui::EndChild();*/
            }
    
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
            }
            if (ImGui::MenuItem("Global build"))
            {
            }
            ImGui::EndMenu();
        }
/*
        static bool open_CreateToolchain = false;
        static bool open_CreateScript = false;
        static bool open_CreateHost = false;
        static bool open_CreateGpos = false;
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


            if (open_CreateHost)
        {
            if (ImGui::BeginPopupModal("Create New Host"))
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

                    //VortexMaker::CreateHost(_label, _author);

                    // open_confirm_popup = true;
                    open_CreateHost = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    open_CreateHost = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

       if (open_CreateGpos)
        {
            if (ImGui::BeginPopupModal("Create New GPOS"))
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

                   // VortexMaker::CreateGpos(_label, _author);

                    // open_confirm_popup = true;
                    open_CreateGpos = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    open_CreateGpos = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }
 if (open_CreateScript)
        {
            if (ImGui::BeginPopupModal("Create New Script"))
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

                   // VortexMaker::CreateScript(_label, _author);

                    // open_confirm_popup = true;
                    open_CreateScript = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    open_CreateScript = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }


        if (open_CreateToolchain)
        {
            if (ImGui::BeginPopupModal("Create New Toolchain"))
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

                   // VortexMaker::CreateToolchain(_label, _author);

                    // open_confirm_popup = true;
                    open_CreateToolchain = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    open_CreateToolchain = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        if (open_CreateToolchain)
            ImGui::OpenPopup("Create New Toolchain");

        if (open_CreateHost)
            ImGui::OpenPopup("Create New Host");

        if (open_CreateGpos)
            ImGui::OpenPopup("Create New GPOS");

        if (open_CreateScript)
            ImGui::OpenPopup("Create New Script");

        if (ImGui::BeginPopupModal("CreationMenu", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Get content");
            ImGui::Separator();
            ImGui::Button("Import...", ImVec2(-1, 0));

            ImGui::Text("Create Basic Component");
            ImGui::Separator();
            // button with full width
            if (ImGui::Button("Toolchain", ImVec2(-1, 50)))
            {
                open_CreateToolchain = true;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Host", ImVec2(-1, 50)))
            {
                open_CreateHost = true;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("General purpose system (GPOS)", ImVec2(-1, 50)))
            {
                open_CreateGpos = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::Button("Package", ImVec2(-1, 50));

            // Image button with text with full width

            ImGui::Text("Create Advanced Component");
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Packages & Tarballs"))
            {
                ImGui::Button("Package (P)", ImVec2(-1, 0));
                ImGui::Button("Package Manager Package (PMP)", ImVec2(-1, 0));
                ImGui::Button("Update Package", ImVec2(-1, 0));
                ImGui::Button("Strapper", ImVec2(-1, 0));
            }

            if (ImGui::CollapsingHeader("Systems"))
            {
                ImGui::Button("General Purpose Operating System (GPOS)", ImVec2(-1, 0));
                ImGui::Button("Real Time Operating System (RTOS)", ImVec2(-1, 0));
                ImGui::Button("Embedded Operating System (EOS)", ImVec2(-1, 0));
                ImGui::Button("Embedded Application (EA)", ImVec2(-1, 0));
            }

            if (ImGui::CollapsingHeader("Scripting"))
            {
                if (ImGui::Button("Simple Script", ImVec2(-1, 0)))
                {
                    open_CreateScript = true;
                    ImGui::CloseCurrentPopup();
                }
            }

            if (ImGui::CollapsingHeader("Toolchains"))
            {
                if (ImGui::Button("Toolchain", ImVec2(-1, 0)))
                {
                    open_CreateToolchain = true;
                    ImGui::CloseCurrentPopup();
                }
            }

            if (ImGui::CollapsingHeader("Debug tools"))
            {
                ImGui::Button("Chrooter", ImVec2(-1, 0));
                ImGui::Button("Testers", ImVec2(-1, 0));
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
*/
        ImGui::EndMenuBar();
    }
}