#include "ToolchainInstance.h"

using namespace VortexMaker;

ToolchainInstance::ToolchainInstance(VxContext *ctx, std::shared_ptr<VxToolchain> _toolchain, InstanceFactory* _factory)
{
  std::cout << "Test" << std::endl;
    this->m_ctx = ctx;
    this->toolchain = _toolchain;
    this->factory = _factory;
    this->toolchain->Init();

  std::cout << "Test" << std::endl;

    this->Refresh();
    this->toolchain->RefreshCurrentWorkingToolchain();

  std::cout << "Test" << std::endl;
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_error, icons::i_error_size, w, h);
        m_ErrorIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_warn, icons::i_warn_size, w, h);
        m_WarningIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_unknow, icons::i_unknow_size, w, h);
        m_UnknowIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_success, icons::i_success_size, w, h);
        m_SuccessIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }

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
        void *data = Walnut::Image::Decode(icons::i_toolchain, icons::i_toolchain_size, w, h);
        m_ToolchainIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
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
        this->UI_TasksEditor();
        this->UI_CurrentToolchainPreview();
        this->UI_SnapshotUtility();
        this->UI_ManualBuild();
        this->UI_TaskLauncher();

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
                static ImTextureID eyeIcon = this->m_EyeIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);



                if(ImGui::ImageButtonWithText(saveIcon, "Save", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    //Save behavior
                } 
                
                if(ImGui::ImageButtonWithText(folderIcon, "Open", ImVec2(this->m_FolderIcon->GetWidth(), this->m_FolderIcon->GetHeight()))){
                    //Save behavior
                }

                ImGui::Separator();

                if (ImGui::BeginMenu("Misc"))
                {
                    if (ImGui::MenuItem("Settings"))
                    {
                    }
                    if (ImGui::MenuItem("Assets"))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Tools"))
                {
                    if (ImGui::MenuItem("Task viewer", NULL, &this->show_UI_TasksEditor))
                    {
                    }
                    if (ImGui::MenuItem("Task launcher", NULL, &this->show_UI_TaskLauncher))
                    {
                    }
                    if (ImGui::MenuItem("Tasklists utility", NULL, &this->show_UI_TasksEditor))
                    {
                    }
                    if (ImGui::MenuItem("Snapshot utility", NULL, &this->show_UI_SnapshotUtility))
                    {
                    }
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if (ImGui::BeginMenu("Build"))
                {
                    if (ImGui::MenuItem("Full build"))
                    {
                    }
                    if (ImGui::MenuItem("Manual build", NULL, &this->show_UI_ManualBuild))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Debug"))
                {
                    if (ImGui::MenuItem("Tester", NULL, &this->show_UI_ParametersWindow))
                    {
                    }
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if(ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight()))){
                }

        if (ImGui::ImageButtonWithText(eyeIcon, "Preview", ImVec2(this->m_EyeIcon->GetWidth(), this->m_EyeIcon->GetHeight())))
        {  
            this->show_UI_CurrentToolchainPreview = !this->show_UI_CurrentToolchainPreview;
        }        
                ImGui::Separator();

                
               

                if(ImGui::ImageButtonWithText(settingsIcon, "Settings", ImVec2(this->m_SettingsIcon->GetWidth(), this->m_SettingsIcon->GetHeight()))){
                    //Save behavior
                }
                ImGui::EndMenuBar();
            }
            
    }


void ToolchainInstance::Refresh()
{
    for(auto task : this->toolchain->tasks){
        task->init();
    }
    //this->m_currentSave = refreshedCurrentSave;
}

void ToolchainInstance::Save()
{
    // Get currentSave (modified by all UI editors)
    // Set host new host variables with save contents
    // Patch json with native Vortex APi
    //this->host->PushSave(this->m_currentSave);
}
