#include "ToolchainInstance.h"

using namespace VortexMaker;

ToolchainInstance::ToolchainInstance(VxContext *ctx, VxToolchain* _toolchain)
{
    this->m_ctx = ctx;
    this->toolchain = _toolchain;


        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_save, icons::i_save_size, w, h);
            m_SaveIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }
        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_add, icons::i_add_size, w, h);
            m_AddIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }
        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_folder, icons::i_folder_size, w, h);
            m_FolderIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }
        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_settings, icons::i_settings_size, w, h);
            m_SettingsIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }
        {
            uint32_t w, h;
            void *data = Walnut::Image::Decode(icons::i_toolchain, icons::i_toolchain_size, w, h);
            m_ToolchainIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
            free(data);
        }

};


void ToolchainInstance::close(){
    this->opened = false;
}


std::string ToolchainInstance::render()
    {
        static ImTextureID toolchainIcon = this->m_ToolchainIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if(opened){
        this->dockspaceID = ImGui::GetID(this->name.c_str());
        static ImGuiIO &io = ImGui::GetIO();

        // Mainwindow with dockspace
        if (ImGui::Begin(name.c_str(), &toolchainIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        {
            this->menubar();
            
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
        }
        else
        {
            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_KeepAliveOnly);
        }

        ImGui::End();

        // All Windows of this instances :
        this->UI_ParametersWindow();
        this->UI_ContentWindow();

        return "rendering";
        }
        else{

        return "closed";
    }
    }



void ToolchainInstance::menubar(){

            if (ImGui::BeginMenuBar())
            {
                static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                static ImTextureID folderIcon = this->m_FolderIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);



                if(ImGui::ImageButtonWithText(saveIcon, "Save", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    //Save behavior
                } 
                
                if(ImGui::ImageButtonWithText(folderIcon, "Open", ImVec2(this->m_FolderIcon->GetWidth(), this->m_FolderIcon->GetHeight()))){
                    //Save behavior
                }

                ImGui::Separator();

                if (ImGui::BeginMenu("Pannels"))
                {
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Tools"))
                {
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    if (ImGui::MenuItem("Contents Window"))
                    {
                    }
                    ImGui::EndMenu();
                }
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
                if (ImGui::BeginMenu("Debug"))
                {
                    if (ImGui::MenuItem("Parameters window", NULL, &this->show_UI_ParametersWindow))
                    {
                    }
                    if (ImGui::MenuItem("Content window", NULL, &this->show_UI_ContentWindow))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Preview"))
                {
                    if (ImGui::MenuItem("Build/Rebuild single parts", NULL, &this->show_UI_ContentWindow))
                    {
                    }
                    if (ImGui::MenuItem("Global build"))
                    {
                    }
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if(ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight()))){
                    //Save behavior
                }
                if (ImGui::Button("Preview"))
                {
                }
                ImGui::Separator();
               

                if(ImGui::ImageButtonWithText(settingsIcon, "Settings", ImVec2(this->m_SettingsIcon->GetWidth(), this->m_SettingsIcon->GetHeight()))){
                    //Save behavior
                }
                ImGui::EndMenuBar();
            }
            
    }
