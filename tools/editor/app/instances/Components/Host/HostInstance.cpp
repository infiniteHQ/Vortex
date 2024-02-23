#include "HostInstance.h"

using namespace VortexMaker;

HostInstance::HostInstance(VxContext *ctx, std::shared_ptr<VxHost> _host, InstanceFactory* _factory)
{
    this->m_ctx = ctx;
    this->host = _host;
    this->factory = _factory;

    // POUR DEMAIN : Finir la gestion et la creation de snapshot et des working host
    // Système de tasks et creation d'actions rapides sur le working host (package, actions, scripts, chroot, etc...)


    this->Refresh();
    this->host->RefreshCurrentWorkingHost();
    this->host->RefreshSnapshots();

    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_save, icons::i_save_size, w, h);
        m_SaveIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_eye, icons::i_eye_size, w, h);
        m_EyeIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
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
        void *data = Walnut::Image::Decode(icons::i_tasklist, icons::i_tasklist_size, w, h);
        m_TaskListIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
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
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_package, icons::i_package_size, w, h);
        m_PackageIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
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
        this->UI_SnapshotUtility();
        this->UI_CurrentHostPreview();
        this->UI_TasksEditor();

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
        static ImTextureID eyeIcon = this->m_EyeIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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
            if (ImGui::MenuItem("Settings", "Main settings of this host", &this->show_UI_MainSettings))
            {
            }
            if (ImGui::MenuItem("Assets", "All contents registered in the host", &this->show_UI_AssetsViewer))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Snapshots", "All saves of this host", &this->show_UI_SnapshotUtility))
            {
            }
            if (ImGui::MenuItem("Build Optimisations", "Optimize the build process"))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Build"))
        {
            if (ImGui::MenuItem("Tasks Editor", "Edit tasks of build processus", &this->show_UI_TasksEditor))
            {
            }
            if (ImGui::MenuItem("Auto Build", "Build from task lists"))
            {
            }
            if (ImGui::MenuItem("Manual Build", "Builds tasks or components manually"))
            {
            }
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
            if (ImGui::MenuItem("Test Window", "Test host, compilers & binaries"))
            {
            }
            if (ImGui::MenuItem("Chroot", "Start a chroot environment"))
            {
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
        {  
            ImGui::OpenPopup("Delete?");
        }       

        if (ImGui::ImageButtonWithText(eyeIcon, "Preview", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
        {  
            this->show_UI_CurrentHostPreview = !this->show_UI_CurrentHostPreview;
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
        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Get content");
            ImGui::Separator();

            ImGui::Text("Create Basic Component");
            ImGui::Separator();

            ImGui::Text("Create Advanced Component");
            ImGui::Separator();

            //static int unused_i = 0;
            //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

            static bool dont_ask_me_next_time = false;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            ImGui::PopStyleVar();

            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
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

    strncpy(refreshedCurrentSave->name, this->host->name.c_str(), sizeof(refreshedCurrentSave->name));
    refreshedCurrentSave->name[sizeof(refreshedCurrentSave->name) - 1] = '\0';

    strncpy(refreshedCurrentSave->author, this->host->author.c_str(), sizeof(refreshedCurrentSave->author));
    refreshedCurrentSave->author[sizeof(refreshedCurrentSave->author) - 1] = '\0';

    strncpy(refreshedCurrentSave->type, this->host->type.c_str(), sizeof(refreshedCurrentSave->type));
    refreshedCurrentSave->type[sizeof(refreshedCurrentSave->type) - 1] = '\0';

    strncpy(refreshedCurrentSave->vendor, this->host->vendor.c_str(), sizeof(refreshedCurrentSave->vendor));
    refreshedCurrentSave->vendor[sizeof(refreshedCurrentSave->vendor) - 1] = '\0';

    strncpy(refreshedCurrentSave->platform, this->host->platform.c_str(), sizeof(refreshedCurrentSave->platform));
    refreshedCurrentSave->platform[sizeof(refreshedCurrentSave->platform) - 1] = '\0';

    strncpy(refreshedCurrentSave->host_arch, this->host->host_arch.c_str(), sizeof(refreshedCurrentSave->host_arch));
    refreshedCurrentSave->host_arch[sizeof(refreshedCurrentSave->host_arch) - 1] = '\0';

    strncpy(refreshedCurrentSave->target_arch, this->host->target_arch.c_str(), sizeof(refreshedCurrentSave->target_arch));
    refreshedCurrentSave->target_arch[sizeof(refreshedCurrentSave->target_arch) - 1] = '\0';

    strncpy(refreshedCurrentSave->toolchainToUse, this->host->toolchainToUse.c_str(), sizeof(refreshedCurrentSave->toolchainToUse));
    refreshedCurrentSave->toolchainToUse[sizeof(refreshedCurrentSave->toolchainToUse) - 1] = '\0';

    strncpy(refreshedCurrentSave->localScriptsPath, this->host->localScriptsPath.c_str(), sizeof(refreshedCurrentSave->localScriptsPath));
    refreshedCurrentSave->localScriptsPath[sizeof(refreshedCurrentSave->localScriptsPath) - 1] = '\0';

    strncpy(refreshedCurrentSave->localPatchsPath, this->host->localPatchsPath.c_str(), sizeof(refreshedCurrentSave->localPatchsPath));
    refreshedCurrentSave->localPatchsPath[sizeof(refreshedCurrentSave->localPatchsPath) - 1] = '\0';

    strncpy(refreshedCurrentSave->localPackagePath, this->host->localPackagePath.c_str(), sizeof(refreshedCurrentSave->localPackagePath));
    refreshedCurrentSave->localPackagePath[sizeof(refreshedCurrentSave->localPackagePath) - 1] = '\0';

    strncpy(refreshedCurrentSave->version, this->host->version.c_str(), sizeof(refreshedCurrentSave->version));
    refreshedCurrentSave->version[sizeof(refreshedCurrentSave->version) - 1] = '\0';

    strncpy(refreshedCurrentSave->description, this->host->description.c_str(), sizeof(refreshedCurrentSave->description));
    refreshedCurrentSave->description[sizeof(refreshedCurrentSave->description) - 1] = '\0';

    // Output the content of buffer
    // refreshedCurrentSave.name = this->host->name;

    for (auto registeredPackage : this->host->registeredPackages)
    {
        std::pair<char[128], char[128]> package;

        strncpy(package.first, registeredPackage->label.c_str(), sizeof(package.first));
        package.first[sizeof(package.first) - 1] = '\0';

        strncpy(package.second, registeredPackage->emplacement.c_str(), sizeof(package.second));
        package.second[sizeof(package.second) - 1] = '\0';

        refreshedCurrentSave->registeredPackages.push_back(package);
    }

    this->m_currentSave = refreshedCurrentSave;
}

void HostInstance::Save()
{
    // Get currentSave (modified by all UI editors)
    // Set host new host variables with save contents
    // Patch json with native Vortex APi
    this->host->PushSave(this->m_currentSave);
}
