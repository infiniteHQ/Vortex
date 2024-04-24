#include "PackageRenderInstance.h"

using namespace VortexMaker;

PackageRenderInstance::PackageRenderInstance(VxContext *ctx, std::shared_ptr<Package> _package)
{
    this->m_ctx = ctx;
    this->package = _package;

    this->Refresh();

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_save, icons::i_save_size, w, h);
        m_SaveIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_folder, icons::i_folder_size, w, h);
        m_FolderIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_settings, icons::i_settings_size, w, h);
        m_SettingsIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_host, icons::i_host_size, w, h);
        m_HostIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_database, icons::i_database_size, w, h);
        m_DatabaseIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_pen, icons::i_pen_size, w, h);
        m_EditIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
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
        void *data = UIKit::Image::Decode(icons::i_build, icons::i_build_size, w, h);
        m_BuildIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_package, icons::i_package_size, w, h);
        m_PackageIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
};

void PackageRenderInstance::close()
{
    this->opened = false;
}

void PackageRenderInstance::render()
{
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID packageIcon = this->m_PackageIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        this->dockspaceID = ImGui::GetID(this->name.c_str());
        static ImGuiIO &io = ImGui::GetIO();

        // Mainwindow with dockspace

        if (ImGui::Begin(name.c_str(), &packageIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        {
            static ImGuiWindow *window = ImGui::GetCurrentWindow();

            this->menubar();

            ImGui::FindWindowByName(name.c_str())->textureID = &packageIcon;

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
        this->UI_MainSettings();
        this->UI_CompilationArguments();
        this->UI_ActionsEditor();


}

void PackageRenderInstance::menubar()
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
            if (ImGui::MenuItem("Compilation Arguments", NULL, &this->show_UI_CompilationArguments))
            {
            }
            if (ImGui::MenuItem("Actions editor", NULL, &this->show_UI_ActionsEditor))
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
            if (ImGui::MenuItem("Global build"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Preview"))
        {
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

void PackageRenderInstance::Refresh()
{
    // Refresh host from vortex API
    // Get all vortex infos, store into save struct and save the save struct to current save struct

    // TODO: BEFORE ALL, REFRESH API INSTANCE OF THIS HOST

    this->package->Refresh();

    std::shared_ptr<PackageSave> refreshedCurrentSave = std::make_shared<PackageSave>();

    strncpy(refreshedCurrentSave->name, this->package->name.c_str(), sizeof(refreshedCurrentSave->name));
    refreshedCurrentSave->name[sizeof(refreshedCurrentSave->name) - 1] = '\0';

    strncpy(refreshedCurrentSave->author, this->package->author.c_str(), sizeof(refreshedCurrentSave->author));
    refreshedCurrentSave->author[sizeof(refreshedCurrentSave->author) - 1] = '\0';

    strncpy(refreshedCurrentSave->description, this->package->description.c_str(), sizeof(refreshedCurrentSave->description));
    refreshedCurrentSave->description[sizeof(refreshedCurrentSave->description) - 1] = '\0';

    strncpy(refreshedCurrentSave->label, this->package->label.c_str(), sizeof(refreshedCurrentSave->label));
    refreshedCurrentSave->label[sizeof(refreshedCurrentSave->label) - 1] = '\0';

    refreshedCurrentSave->priority = this->package->priority;

    strncpy(refreshedCurrentSave->compressed, this->package->compressed.c_str(), sizeof(refreshedCurrentSave->compressed));
    refreshedCurrentSave->compressed[sizeof(refreshedCurrentSave->compressed) - 1] = '\0';

    strncpy(refreshedCurrentSave->filename, this->package->fileName.c_str(), sizeof(refreshedCurrentSave->filename));
    refreshedCurrentSave->filename[sizeof(refreshedCurrentSave->filename) - 1] = '\0';



    strncpy(refreshedCurrentSave->useOnlyCustomConfigurationProcess, this->package->compilation.customConfigurationProcess.c_str(), sizeof(refreshedCurrentSave->useOnlyCustomConfigurationProcess));
    refreshedCurrentSave->useOnlyCustomConfigurationProcess[sizeof(refreshedCurrentSave->useOnlyCustomConfigurationProcess) - 1] = '\0';

    strncpy(refreshedCurrentSave->useOnlyCustomCompilationProcess, this->package->compilation.customCompilationProcess.c_str(), sizeof(refreshedCurrentSave->useOnlyCustomCompilationProcess));
    refreshedCurrentSave->useOnlyCustomCompilationProcess[sizeof(refreshedCurrentSave->useOnlyCustomCompilationProcess) - 1] = '\0';

    strncpy(refreshedCurrentSave->useOnlyCustomInstallationProcess, this->package->compilation.customInstallationProcess.c_str(), sizeof(refreshedCurrentSave->useOnlyCustomInstallationProcess));
    refreshedCurrentSave->useOnlyCustomInstallationProcess[sizeof(refreshedCurrentSave->useOnlyCustomInstallationProcess) - 1] = '\0';


    refreshedCurrentSave->useCompilationOptimization = this->package->compilation.useCompilationOptimization;


    // Output the content of buffer
    // refreshedCurrentSave.name = this->host->name;

    for (auto configurationPrefix : this->package->compilation.configurationPrefixes)
    {
        std::pair<char[128], char[128]> package;

        strncpy(package.first, configurationPrefix.first.c_str(), sizeof(package.first));
        package.first[sizeof(package.first) - 1] = '\0';

        strncpy(package.second, configurationPrefix.second.c_str(), sizeof(package.second));
        package.second[sizeof(package.second) - 1] = '\0';
        
        refreshedCurrentSave->configurationPrefixes.push_back(package);
    }

    for (auto compilationPrefix : this->package->compilation.compilationPrefixes)
    {
        std::pair<char[128], char[128]> package;

        strncpy(package.first, compilationPrefix.first.c_str(), sizeof(package.first));
        package.first[sizeof(package.first) - 1] = '\0';

        strncpy(package.second, compilationPrefix.second.c_str(), sizeof(package.second));
        package.second[sizeof(package.second) - 1] = '\0';
        
        refreshedCurrentSave->compilationPrefixes.push_back(package);
    }

    for (auto installationPrefix : this->package->compilation.installationPrefixes)
    {
        std::pair<char[128], char[128]> package;

        strncpy(package.first, installationPrefix.first.c_str(), sizeof(package.first));
        package.first[sizeof(package.first) - 1] = '\0';

        strncpy(package.second, installationPrefix.second.c_str(), sizeof(package.second));
        package.second[sizeof(package.second) - 1] = '\0';
        
        refreshedCurrentSave->installationPrefixes.push_back(package);
    }

    ////////////////////////////////////

    for (auto configurationSuffix : this->package->compilation.configurationSuffixes)
    {
        std::pair<char[128], char[128]> package;

        strncpy(package.first, configurationSuffix.first.c_str(), sizeof(package.first));
        package.first[sizeof(package.first) - 1] = '\0';

        strncpy(package.second, configurationSuffix.second.c_str(), sizeof(package.second));
        package.second[sizeof(package.second) - 1] = '\0';
        
        refreshedCurrentSave->configurationSuffixes.push_back(package);
    }

    for (auto compilationSuffix : this->package->compilation.compilationSuffixes)
    {
        std::pair<char[128], char[128]> package;

        strncpy(package.first, compilationSuffix.first.c_str(), sizeof(package.first));
        package.first[sizeof(package.first) - 1] = '\0';

        strncpy(package.second, compilationSuffix.second.c_str(), sizeof(package.second));
        package.second[sizeof(package.second) - 1] = '\0';
        
        refreshedCurrentSave->compilationSuffixes.push_back(package);
    }

    for (auto installationSuffix : this->package->compilation.installationSuffixes)
    {
        std::pair<char[128], char[128]> package;

        strncpy(package.first, installationSuffix.first.c_str(), sizeof(package.first));
        package.first[sizeof(package.first) - 1] = '\0';

        strncpy(package.second, installationSuffix.second.c_str(), sizeof(package.second));
        package.second[sizeof(package.second) - 1] = '\0';
        
        refreshedCurrentSave->installationSuffixes.push_back(package);
    }



    strncpy(refreshedCurrentSave->compilationExclusiveCommand, this->package->compilation.exclusiveCustomCompilationProcess.c_str(), sizeof(refreshedCurrentSave->compilationExclusiveCommand));
    refreshedCurrentSave->compilationExclusiveCommand[sizeof(refreshedCurrentSave->compilationExclusiveCommand) - 1] = '\0';

    strncpy(refreshedCurrentSave->configurationExclusiveCommand, this->package->compilation.exclusiveCustomConfigProcess.c_str(), sizeof(refreshedCurrentSave->configurationExclusiveCommand));
    refreshedCurrentSave->configurationExclusiveCommand[sizeof(refreshedCurrentSave->configurationExclusiveCommand) - 1] = '\0';

    strncpy(refreshedCurrentSave->installationExclusiveCommand, this->package->compilation.exclusiveCustomInstallationProcess.c_str(), sizeof(refreshedCurrentSave->installationExclusiveCommand));
    refreshedCurrentSave->installationExclusiveCommand[sizeof(refreshedCurrentSave->installationExclusiveCommand) - 1] = '\0';

    ////////////////////////////////////

    for (auto action : this->package->actions)
    {
        PackageActionSave savedAction;

        strncpy(savedAction.command, action->command.c_str(), sizeof(savedAction.command));
        savedAction.command[sizeof(savedAction.command) - 1] = '\0';

        savedAction.priority = action->priority;

        strncpy(savedAction.sequence, action->executionSequence.c_str(), sizeof(savedAction.sequence));
        savedAction.sequence[sizeof(savedAction.sequence) - 1] = '\0';
        
        strncpy(savedAction.type, action->type.c_str(), sizeof(savedAction.type));
        savedAction.type[sizeof(savedAction.type) - 1] = '\0';
        
        refreshedCurrentSave->actions.push_back(savedAction);
    }

    this->m_currentSave = refreshedCurrentSave;
 
}

void PackageRenderInstance::Save()
{
    // Get currentSave (modified by all UI editors)
    // Set host new host variables with save contents
    // Patch json with native Vortex APi
    this->package->PushSave(this->m_currentSave);
}
