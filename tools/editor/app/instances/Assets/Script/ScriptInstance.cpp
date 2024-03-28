#include "ScriptInstance.h"

using namespace VortexMaker;

ScriptInstance::ScriptInstance(VxContext *ctx, std::shared_ptr<VxScript> _script)
{
    this->m_ctx = ctx;
    this->script = _script;

    this->Refresh();

    this->editor = std::make_shared<TextEditor>(ctx, this->script->path + "/pool/main.sh");


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
        void *data = Walnut::Image::Decode(icons::i_script, icons::i_script_size, w, h);
        m_ScriptIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
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

void ScriptInstance::close()
{
    this->opened = false;
}

std::string ScriptInstance::render()
{
    if (this->opened)
    {

    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID packageIcon = this->m_PackageIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID scriptIcon = this->m_ScriptIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        this->dockspaceID = ImGui::GetID(this->name.c_str());
        static ImGuiIO &io = ImGui::GetIO();

        // Mainwindow with dockspace

        if (ImGui::Begin(name.c_str(), &scriptIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        {
            static ImGuiWindow *window = ImGui::GetCurrentWindow();

            this->menubar();

            ImGui::FindWindowByName(name.c_str())->textureID = &scriptIcon;

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
        this->UI_TextEditor();


        return "rendering";
    }
    else
    {
        return "closed";
    }
}

void ScriptInstance::menubar()
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
            if (ImGui::MenuItem("Text Editor", NULL, &this->show_UI_TextEditor))
            {
            }
            if (ImGui::MenuItem("Main Settings", NULL, &this->show_UI_MainSettings))
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

void ScriptInstance::Refresh()
{
    // Refresh host from vortex API
    // Get all vortex infos, store into save struct and save the save struct to current save struct

    // TODO: BEFORE ALL, REFRESH API INSTANCE OF THIS HOST

    this->script->Refresh();

    std::shared_ptr<VxScriptSave> refreshedCurrentSave = std::make_shared<VxScriptSave>();

    strncpy(refreshedCurrentSave->name, this->script->name.c_str(), sizeof(refreshedCurrentSave->name));
    refreshedCurrentSave->name[sizeof(refreshedCurrentSave->name) - 1] = '\0';

    strncpy(refreshedCurrentSave->author, this->script->author.c_str(), sizeof(refreshedCurrentSave->author));
    refreshedCurrentSave->author[sizeof(refreshedCurrentSave->author) - 1] = '\0';

    this->m_currentSave = refreshedCurrentSave;
}

void ScriptInstance::Save()
{
    // Get currentSave (modified by all UI editors)
    // Set host new host variables with save contents
    // Patch json with native Vortex APi
    this->script->PushSave(this->m_currentSave);
}
