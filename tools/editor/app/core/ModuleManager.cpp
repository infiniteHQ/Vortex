#include "ModuleManager.hpp"

#include <iostream>

static int item_current = 0;

ModuleManager::ModuleManager(VxContext *_ctx, InstanceFactory *_factory)
{
    this->ctx = _ctx;
    this->factory = _factory;
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
        void *data = Walnut::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
}

void ModuleManager::OnImGuiRender()
{
    static ImTextureID listIcon = this->m_ListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::Begin("Modules manager", &listIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        this->menubar();

    float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.3;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module of ");
    ImGui::SameLine();
    ImGui::Text(this->ctx->name.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    ImGui::Separator();

    // Left
    static int selected = 0;
    static std::array<char[128], 3> labels = {"Instances", "Utils", "Workers"};

    {
        ImGui::BeginChild("left pane", ImVec2(230, -1), true);
        for (int i = 0; i < labels.size(); i++)
        {
            if (i == 0)
            {
                std::string label = "All modules (" + std::to_string(this->ctx->IO.em.size()) + ")";
                ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), label.c_str());
            }

            std::string label;

            switch (i)
            {
            case 0:
            {

                int number = 0;
                for (auto em : this->ctx->IO.em)
                {
                    if (em->m_type == "em_instanciable")
                    {
                        number++;
                    }
                }
                label = labels[i];
                label += " (";
                label += std::to_string(number);
                label += ")";
                break; 
            }
            case 1:
            {

                int number = 0;
                for (auto em : this->ctx->IO.em)
                {
                    if (em->m_type == "em_instanciable")
                    {
                        number++;
                    }
                }
                label = labels[i];
                label += " (";
                label += std::to_string(number);
                label += ")";
                break; 
            }
            case 2:
            {

                int number = 0;
                for (auto em : this->ctx->IO.em)
                {
                    if (em->m_type == "other..")
                    {
                        number++;
                    }
                }
                label = labels[i];
                label += " (";
                label += std::to_string(number);
                label += ")";
                break; 
            }
            }

            label += "##";
            label += labels[i];

            if (ImGui::Selectable(label.c_str(), selected == i))
                selected = i;
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();

    for (int row = 0; row < this->ctx->IO.em.size(); row++)
    {
        switch (selected)
        {
        case 1:
        {
            if (this->ctx->IO.em[row]->m_type == "em_instanciable")
            {

                std::string childLabel = "module##" + this->ctx->IO.em[row]->m_name;

                ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 200), true);

                {
                    uint32_t w, h;
                    void *data = Walnut::Image::Decode(this->ctx->IO.em[row]->m_logo, this->ctx->IO.em[row]->m_logo_size, w, h);
                    static std::shared_ptr<Walnut::Image> _icon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
                    free(data);
                    static ImTextureID addIcon = _icon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                    ImGui::Image(addIcon, ImVec2(50, 50));
                    ImGui::SameLine();
                }

                {

                    float oldsize = ImGui::GetFont()->Scale;
                    ImGui::GetFont()->Scale *= 1.3;
                    ImGui::PushFont(ImGui::GetFont());

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), this->ctx->IO.em[row]->m_proper_name.c_str());

                    ImGui::GetFont()->Scale = oldsize;
                    ImGui::PopFont();
                }

                {

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) : ");
                    ImGui::SameLine();
                    ImGui::Text(this->ctx->IO.em[row]->m_author.c_str());
                }

                {

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description : ");
                    ImGui::SameLine();
                    ImGui::Text(this->ctx->IO.em[row]->m_description.c_str());
                }

                {

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Contributors : ");
                    int i = 0;
                    for (auto contributor : this->ctx->IO.em[row]->m_contributors)
                    {
                        i++;
                        if (i <= 3)
                        {
                            if (i < this->ctx->IO.em[row]->m_contributors.size())
                            {
                                ImGui::SameLine();
                                std::string contri = contributor + ",";
                                ImGui::Text(contri.c_str());
                            }
                            else
                            {
                                ImGui::SameLine();
                                ImGui::Text(contributor.c_str());
                            }
                        }
                        else
                        {
                            ImGui::SameLine();
                            int counter = this->ctx->IO.em[row]->m_contributors.size() - 3;
                            std::string label = " and " + std::to_string(counter) + " other...";
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
                            break;
                        }
                    }
                }

                if (ImGui::ImageButtonWithText(listIcon, "Details", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {

                    std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(ctx, this->ctx->IO.em[row]);
                    factory->SpawnInstance(instance);
                }

                if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                }
            }
            ImGui::EndChild();
        }
        }
    }

    ImGui::End();
}

void ModuleManager::menubar()
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

                    VortexMaker::CreateHost(_label, _author);

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

                    VortexMaker::CreateGpos(_label, _author);

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

                    VortexMaker::CreateScript(_label, _author);

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

                    VortexMaker::CreateToolchain(_label, _author);

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

        ImGui::EndMenuBar();
    }
}