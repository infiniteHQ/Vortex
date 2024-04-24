#include "ToolchainRenderInstance.h"

using namespace VortexMaker;

ToolchainRenderInstance::ToolchainRenderInstance(VxContext *ctx, std::shared_ptr<Toolchain> _toolchain)
{
    this->m_ctx = ctx;
    this->toolchain = _toolchain;

    this->Refresh();
    this->toolchain->RefreshCurrentWorkingToolchain();
    this->toolchain->RefreshSnapshots();

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_error, icons::i_error_size, w, h);
        m_ErrorIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_warn, icons::i_warn_size, w, h);
        m_WarningIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_unknow, icons::i_unknow_size, w, h);
        m_UnknowIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_success, icons::i_success_size, w, h);
        m_SuccessIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_save, icons::i_save_size, w, h);
        m_SaveIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_eye, icons::i_eye_size, w, h);
        m_EyeIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
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
        void *data = UIKit::Image::Decode(icons::i_tasklist, icons::i_tasklist_size, w, h);
        m_TaskListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
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
        void *data = UIKit::Image::Decode(icons::i_toolchain, icons::i_toolchain_size, w, h);
        m_ToolchainIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
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
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_logs, icons::i_logs_size, w, h);
        m_FlipBookIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
};

void ToolchainRenderInstance::close()
{
    this->opened = false;
}

void ToolchainRenderInstance::render()
{
    static ImTextureID toolchainIcon = this->m_ToolchainIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    this->dockspaceID = ImGui::GetID(this->toolchain->name.c_str());
    static ImGuiIO &io = ImGui::GetIO();

    // Mainwindow with dockspace
    if (ImGui::Begin(this->toolchain->name.c_str(), &toolchainIcon, &this->opened, ImGuiWindowFlags_MenuBar))
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
    this->UI_AssetsViewer();
    this->UI_VolatileTasks();
    this->UI_CurrentToolchainPreview();
    this->UI_TasksEditor();
    this->UI_SnapshotUtility();
    this->UI_MainSettings();
    this->UI_FullBuild();
    this->UI_ManualBuild();
}

void ToolchainRenderInstance::menubar()
{

    if (ImGui::BeginMenuBar())
    {

        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID folderIcon = this->m_FolderIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID eyeIcon = this->m_EyeIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if (ImGui::ImageButtonWithText(saveIcon, "Save", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
        {
            // Save behavior
        }

        if (ImGui::ImageButtonWithText(folderIcon, "Open", ImVec2(this->m_FolderIcon->GetWidth(), this->m_FolderIcon->GetHeight())))
        {
            // Save behavior
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("Misc"))
        {
            if (ImGui::MenuItem("Settings", NULL, &this->show_UI_MainSettings))
            {
            }
            if (ImGui::MenuItem("Assets", NULL, &this->show_UI_AssetsViewer))
            {
            }
            if (ImGui::MenuItem("Tasks", NULL, &this->show_UI_TasksEditor))
            {
            }
            if (ImGui::MenuItem("Content", NULL, &this->show_UI_ContentWindow))
            {
            }
            if (ImGui::MenuItem("Params", NULL, &this->show_UI_ParametersWindow))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Volatile tasks", NULL, &this->show_UI_VolatileTasks))
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
            if (ImGui::MenuItem("Full build", NULL, &this->show_UI_FullBuild))
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

        if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
        {

            ImGui::OpenPopup("Add a composant");
        }

        if (ImGui::ImageButtonWithText(eyeIcon, "Preview", ImVec2(this->m_EyeIcon->GetWidth(), this->m_EyeIcon->GetHeight())))
        {
            this->show_UI_CurrentToolchainPreview = !this->show_UI_CurrentToolchainPreview;
        }
        ImGui::Separator();

        if (ImGui::ImageButtonWithText(settingsIcon, "Settings", ImVec2(this->m_SettingsIcon->GetWidth(), this->m_SettingsIcon->GetHeight())))
        {
            // Save behavior
        }

        static bool open_ImportationMenu = false;
        static bool open_CreateTaskList = false;

        if (open_ImportationMenu)
        {
            if (ImGui::BeginPopupModal("Import content(s)", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("From this project");
                ImGui::Separator();

                // Image button with text with full width

                ImGui::Text("Create Advanced Component");
                ImGui::Separator();

                if (ImGui::CollapsingHeader("Import package(s)"))
                {
                    /* FIXME for(auto package : this->m_ctx->IO.packages){
                         if (ImGui::Button(package->label.c_str(), ImVec2(-1, 0)))
                         {
                             std::pair<char[128],char[128]> pair;
                             std::strcpy(pair.first, package->label.c_str());
                             std::strcpy(pair.second, "global");
                             this->m_currentSave->registeredPackages.push_back(pair);
                             this->Save();

                             this->Refresh();

                         open_ImportationMenu = false;
                             ImGui::CloseCurrentPopup();
                         }
                     }*/
                }

                // static int unused_i = 0;
                // ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    open_ImportationMenu = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    open_ImportationMenu = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (open_CreateTaskList)
        {
            if (ImGui::BeginPopupModal("Create new Tasklist", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {

                // 3 text inputs
                static std::pair<char[128], char[128]> pair;
                // inputs widget
                ImGui::InputText("Tasklist Name", pair.first, IM_ARRAYSIZE(pair.first));
                // std::string _TasklistName = TasklistName;

                // static int unused_i = 0;
                // ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

                if (ImGui::Button("Create", ImVec2(120, 0)))
                {
                    this->toolchain->CreateTasklist(pair.first, this->m_currentSave);
                    this->m_currentSave->registeredTasklists.push_back(pair);
                    this->Save();
                    this->Refresh();
                    open_CreateTaskList = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    open_CreateTaskList = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        if (open_ImportationMenu)
            ImGui::OpenPopup("Import content(s)");

        if (open_CreateTaskList)
            ImGui::OpenPopup("Create new Tasklist");

        if (ImGui::BeginPopupModal("Add a composant", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Get content");
            ImGui::Separator();
            if (ImGui::Button("Import...", ImVec2(-1, 0)))
            {

                open_ImportationMenu = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::Text("Create Basic Component");
            ImGui::Separator();
            // Image button with text with full width

            ImGui::Text("Create Advanced Component");
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Tasklists"))
            {
                if (ImGui::Button("Tasklist (TL)", ImVec2(-1, 0)))
                {
                    open_CreateTaskList = true;
                    ImGui::CloseCurrentPopup();
                }
            }

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

void ToolchainRenderInstance::Refresh()
{
    // Refresh host from vortex API
    // Get all vortex infos, store into save struct and save the save struct to current save struct

    // TODO: BEFORE ALL, REFRESH API INSTANCE OF THIS HOST

    VortexMaker::LogInfo("Core", "Start refreshing " + this->name);
    this->toolchain->Refresh();
    this->toolchain->RefreshDistConfig();
    // Refresh dist

    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    args->add("pool_name", this->toolchain->pool_name);
    VortexMaker::CallModuleEvent(args, "GetTaskPool", "vortex.modules.builtin.tasks");
    std::shared_ptr<TaskPool> list = args->get<std::shared_ptr<TaskPool>>("taskpool", nullptr);
    VortexMaker::LogInfo("Core", "Init core tasks of " + this->name);

    if (list != nullptr)
    {
        for (auto task : list->m_list)
        {
            task->init();
        }
        // tasklist = list->m_list;
    }

    VortexMaker::LogInfo("Core", "Load a new save " + this->name);
    std::shared_ptr<ToolchainSave> refreshedCurrentSave = std::make_shared<ToolchainSave>();
    std::shared_ptr<VxDistToolchainSave> refreshedCurrentDistSave = std::make_shared<VxDistToolchainSave>();

    strncpy(refreshedCurrentSave->name, this->toolchain->name.c_str(), sizeof(refreshedCurrentSave->name));
    refreshedCurrentSave->name[sizeof(refreshedCurrentSave->name) - 1] = '\0';

    strncpy(refreshedCurrentSave->author, this->toolchain->author.c_str(), sizeof(refreshedCurrentSave->author));
    refreshedCurrentSave->author[sizeof(refreshedCurrentSave->author) - 1] = '\0';

    strncpy(refreshedCurrentSave->type, this->toolchain->type.c_str(), sizeof(refreshedCurrentSave->type));
    refreshedCurrentSave->type[sizeof(refreshedCurrentSave->type) - 1] = '\0';

    strncpy(refreshedCurrentSave->description, this->toolchain->description.c_str(), sizeof(refreshedCurrentSave->description));
    refreshedCurrentSave->description[sizeof(refreshedCurrentSave->description) - 1] = '\0';

    strncpy(refreshedCurrentSave->version, this->toolchain->version.c_str(), sizeof(refreshedCurrentSave->version));
    refreshedCurrentSave->version[sizeof(refreshedCurrentSave->version) - 1] = '\0';

    strncpy(refreshedCurrentSave->state, this->toolchain->state.c_str(), sizeof(refreshedCurrentSave->state));
    refreshedCurrentSave->state[sizeof(refreshedCurrentSave->state) - 1] = '\0';

    strncpy(refreshedCurrentSave->host_arch, this->toolchain->host_arch.c_str(), sizeof(refreshedCurrentSave->host_arch));
    refreshedCurrentSave->host_arch[sizeof(refreshedCurrentSave->host_arch) - 1] = '\0';

    strncpy(refreshedCurrentSave->host_vendor, this->toolchain->host_vendor.c_str(), sizeof(refreshedCurrentSave->host_vendor));
    refreshedCurrentSave->host_vendor[sizeof(refreshedCurrentSave->host_vendor) - 1] = '\0';

    strncpy(refreshedCurrentSave->host_platform, this->toolchain->host_platform.c_str(), sizeof(refreshedCurrentSave->host_platform));
    refreshedCurrentSave->host_platform[sizeof(refreshedCurrentSave->host_platform) - 1] = '\0';

    strncpy(refreshedCurrentSave->host_cpu, this->toolchain->host_cpu.c_str(), sizeof(refreshedCurrentSave->host_cpu));
    refreshedCurrentSave->host_cpu[sizeof(refreshedCurrentSave->host_cpu) - 1] = '\0';

    strncpy(refreshedCurrentSave->host_fpu, this->toolchain->host_fpu.c_str(), sizeof(refreshedCurrentSave->host_fpu));
    refreshedCurrentSave->host_fpu[sizeof(refreshedCurrentSave->host_fpu) - 1] = '\0';

    strncpy(refreshedCurrentSave->target_arch, this->toolchain->target_arch.c_str(), sizeof(refreshedCurrentSave->target_arch));
    refreshedCurrentSave->target_arch[sizeof(refreshedCurrentSave->target_arch) - 1] = '\0';

    strncpy(refreshedCurrentSave->target_vendor, this->toolchain->target_vendor.c_str(), sizeof(refreshedCurrentSave->target_vendor));
    refreshedCurrentSave->target_vendor[sizeof(refreshedCurrentSave->target_vendor) - 1] = '\0';

    strncpy(refreshedCurrentSave->target_platform, this->toolchain->target_platform.c_str(), sizeof(refreshedCurrentSave->target_platform));
    refreshedCurrentSave->target_platform[sizeof(refreshedCurrentSave->target_platform) - 1] = '\0';

    strncpy(refreshedCurrentSave->target_cpu, this->toolchain->target_cpu.c_str(), sizeof(refreshedCurrentSave->target_cpu));
    refreshedCurrentSave->target_cpu[sizeof(refreshedCurrentSave->target_cpu) - 1] = '\0';

    strncpy(refreshedCurrentSave->target_fpu, this->toolchain->target_fpu.c_str(), sizeof(refreshedCurrentSave->target_fpu));
    refreshedCurrentSave->target_fpu[sizeof(refreshedCurrentSave->target_fpu) - 1] = '\0';

    strncpy(refreshedCurrentSave->builder_arch, this->toolchain->builder_arch.c_str(), sizeof(refreshedCurrentSave->builder_arch));
    refreshedCurrentSave->builder_arch[sizeof(refreshedCurrentSave->builder_arch) - 1] = '\0';

    strncpy(refreshedCurrentSave->builder_vendor, this->toolchain->builder_vendor.c_str(), sizeof(refreshedCurrentSave->builder_vendor));
    refreshedCurrentSave->builder_vendor[sizeof(refreshedCurrentSave->builder_vendor) - 1] = '\0';

    strncpy(refreshedCurrentSave->builder_platform, this->toolchain->builder_platform.c_str(), sizeof(refreshedCurrentSave->builder_platform));
    refreshedCurrentSave->builder_platform[sizeof(refreshedCurrentSave->builder_platform) - 1] = '\0';

    strncpy(refreshedCurrentSave->builder_cpu, this->toolchain->builder_cpu.c_str(), sizeof(refreshedCurrentSave->builder_cpu));
    refreshedCurrentSave->builder_cpu[sizeof(refreshedCurrentSave->builder_cpu) - 1] = '\0';

    strncpy(refreshedCurrentSave->builder_fpu, this->toolchain->builder_fpu.c_str(), sizeof(refreshedCurrentSave->builder_fpu));
    refreshedCurrentSave->builder_fpu[sizeof(refreshedCurrentSave->builder_fpu) - 1] = '\0';

    strncpy(refreshedCurrentSave->compression, this->toolchain->compressionMode.c_str(), sizeof(refreshedCurrentSave->compression));
    refreshedCurrentSave->compression[sizeof(refreshedCurrentSave->compression) - 1] = '\0';

    strncpy(refreshedCurrentSave->localScriptsPath, this->toolchain->localScriptsPath.c_str(), sizeof(refreshedCurrentSave->localScriptsPath));
    refreshedCurrentSave->localScriptsPath[sizeof(refreshedCurrentSave->localScriptsPath) - 1] = '\0';

    strncpy(refreshedCurrentSave->localPatchsPath, this->toolchain->localPatchsPath.c_str(), sizeof(refreshedCurrentSave->localPatchsPath));
    refreshedCurrentSave->localPatchsPath[sizeof(refreshedCurrentSave->localPatchsPath) - 1] = '\0';

    strncpy(refreshedCurrentSave->localPackagePath, this->toolchain->localPackagesPath.c_str(), sizeof(refreshedCurrentSave->localPackagePath));
    refreshedCurrentSave->localPackagePath[sizeof(refreshedCurrentSave->localPackagePath) - 1] = '\0';

    // Output the content of buffer
    // refreshedCurrentSave.name = this->toolchain->name;

    for (auto registeredPackage : this->toolchain->registeredPackages)
    {
        std::pair<char[128], char[128]> package;

        strncpy(package.first, registeredPackage->label.c_str(), sizeof(package.first));
        package.first[sizeof(package.first) - 1] = '\0';

        strncpy(package.second, registeredPackage->emplacement.c_str(), sizeof(package.second));
        package.second[sizeof(package.second) - 1] = '\0';

        refreshedCurrentSave->registeredPackages.push_back(package);
    }
        for(auto registeredTasklists : this->toolchain->registeredTasklists){
            std::pair<char[128], char[128]> tasklist;

            strncpy(tasklist.first, registeredTasklists->label.c_str(), sizeof(tasklist.first));
            tasklist.first[sizeof(tasklist.first) - 1] = '\0';


            refreshedCurrentSave->registeredTasklists.push_back(tasklist);
        }   

    strncpy(refreshedCurrentDistSave->AR_value, this->toolchain->distToolchain.AR.c_str(), sizeof(refreshedCurrentDistSave->AR_value));
    refreshedCurrentDistSave->AR_value[sizeof(refreshedCurrentDistSave->AR_value) - 1] = '\0';

    strncpy(refreshedCurrentDistSave->AS_value, this->toolchain->distToolchain.AS.c_str(), sizeof(refreshedCurrentDistSave->AS_value));
    refreshedCurrentDistSave->AS_value[sizeof(refreshedCurrentDistSave->AS_value) - 1] = '\0';

    strncpy(refreshedCurrentDistSave->CC_value, this->toolchain->distToolchain.CC.c_str(), sizeof(refreshedCurrentDistSave->CC_value));
    refreshedCurrentDistSave->CC_value[sizeof(refreshedCurrentDistSave->CC_value) - 1] = '\0';

    strncpy(refreshedCurrentDistSave->CXX_value, this->toolchain->distToolchain.CXX.c_str(), sizeof(refreshedCurrentDistSave->CXX_value));
    refreshedCurrentDistSave->CXX_value[sizeof(refreshedCurrentDistSave->CXX_value) - 1] = '\0';

    strncpy(refreshedCurrentDistSave->LD_value, this->toolchain->distToolchain.LD.c_str(), sizeof(refreshedCurrentDistSave->LD_value));
    refreshedCurrentDistSave->LD_value[sizeof(refreshedCurrentDistSave->LD_value) - 1] = '\0';

    strncpy(refreshedCurrentDistSave->RANLIB_value, this->toolchain->distToolchain.RANLIB.c_str(), sizeof(refreshedCurrentDistSave->RANLIB_value));
    refreshedCurrentDistSave->RANLIB_value[sizeof(refreshedCurrentDistSave->RANLIB_value) - 1] = '\0';

    strncpy(refreshedCurrentDistSave->STRIP_value, this->toolchain->distToolchain.STRIP.c_str(), sizeof(refreshedCurrentDistSave->STRIP_value));
    refreshedCurrentDistSave->STRIP_value[sizeof(refreshedCurrentDistSave->STRIP_value) - 1] = '\0';

    this->m_currentSave = refreshedCurrentSave;
    this->m_currentDistSave = refreshedCurrentDistSave;
}

void ToolchainRenderInstance::Save()
{
    VortexMaker::LogInfo("Core", "Save... " + this->name);
    // Get currentSave (modified by all UI editors)
    // Set host new host variables with save contents
    // Patch json with native Vortex APi
    this->toolchain->PushSave(this->m_currentSave);
    this->toolchain->PushDistSave(this->m_currentDistSave);
}