#include "HostInstance.h"

using namespace VortexMaker;

HostInstance::HostInstance(VxContext *ctx, std::shared_ptr<VxHost> _host)
{
    this->m_ctx = ctx;
    this->host = _host;

    this->Refresh();

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
        void *data = Walnut::Image::Decode(icons::i_host, icons::i_host_size, w, h);
        m_HostIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_database, icons::i_database_size, w, h);
        m_DatabaseIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_pen, icons::i_pen_size, w, h);
        m_EditIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
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
        void *data = Walnut::Image::Decode(icons::i_build, icons::i_build_size, w, h);
        m_BuildIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
};

void HostInstance::close()
{
    this->opened = false;
}

std::string HostInstance::render()
{
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID hostIcon = this->m_HostIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    if (opened)
    {

        this->dockspaceID = ImGui::GetID(this->name.c_str());
        static ImGuiIO &io = ImGui::GetIO();

        // Mainwindow with dockspace
        if (ImGui::Begin(name.c_str(), &hostIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        {
            static ImGuiWindow *window = ImGui::GetCurrentWindow();

            this->menubar();

            ImGui::FindWindowByName(name.c_str())->textureID = &hostIcon;

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
        this->UI_MainSettings();
        this->UI_AssetsViewer();
        this->UI_FullBuild();

        return "rendering";
    }
    else
    {

        return "closed";
    }
}

void HostInstance::menubar()
{

    if (ImGui::BeginMenuBar())
    {
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID folderIcon = this->m_FolderIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if (ImGui::ImageButtonWithText(saveIcon, "Save All", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
        {
            // Save behavior
        }

        if (ImGui::ImageButtonWithText(folderIcon, "Open", ImVec2(this->m_FolderIcon->GetWidth(), this->m_FolderIcon->GetHeight())))
        {
            // Save behavior
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("Pannels"))
        {
            if (ImGui::MenuItem("Main Settings", NULL, &this->show_UI_MainSettings))
            {
            }
            if (ImGui::MenuItem("Options Editor", NULL, &this->show_UI_AssetsViewer))
            {
            }
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
            if (ImGui::MenuItem("Full Build", NULL, &this->show_UI_FullBuild))
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

        if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
        {
            // Save behavior
        }
        if (ImGui::Button("Preview"))
        {
        }
        ImGui::Separator();

        if (ImGui::ImageButtonWithText(settingsIcon, "Settings", ImVec2(this->m_SettingsIcon->GetWidth(), this->m_SettingsIcon->GetHeight())))
        {
            // Save behavior
        }

        if (ImGui::ImageButtonWithText(settingsIcon, "Close", ImVec2(this->m_SettingsIcon->GetWidth(), this->m_SettingsIcon->GetHeight())))
        {
            this->close();
        }
        ImGui::EndMenuBar();
    }
}

void HostInstance::Refresh()
{
    // Refresh host from vortex API
    // Get all vortex infos, store into save struct and save the save struct to current save struct


    // TODO: BEFORE ALL, REFRESH API INSTANCE OF THIS HOST
    this->host->Refresh();

    std::shared_ptr<HostSave> refreshedCurrentSave = std::make_shared<HostSave>();


    // Copy the content of str into buffer, ensuring null-termination
    strncpy(refreshedCurrentSave->name, this->host->name.c_str(), sizeof(refreshedCurrentSave->name)); // Copy up to sizeof(buffer) characters
    refreshedCurrentSave->name[sizeof(refreshedCurrentSave->name) - 1] = '\0'; // Null-terminate the buffer




    // Output the content of buffer
    //refreshedCurrentSave.name = this->host->name;

    this->m_currentSave = refreshedCurrentSave;
}

void HostInstance::Save(){
    // Get currentSave (modified by all UI editors)
    // Set host new host variables with save contents
    // Patch json with native Vortex APi
}

