#include "ReportInstance.h"

using namespace VortexMaker;

ReportInstance::ReportInstance(VxContext *ctx, std::shared_ptr<Task> _task)
{
    this->m_ctx = ctx;
    this->task = _task;

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
        void *data = Walnut::Image::Decode(icons::i_list, icons::i_list_size, w, h);
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


void ReportInstance::close(){
    this->opened = false;
}


std::string ReportInstance::render()
    {
        static ImTextureID toolchainIcon = this->m_ToolchainIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if(opened){
        // Mainwindow with dockspace
        static std::string label = "Reporting task" + this->name;
        if (ImGui::Begin(label.c_str(), &toolchainIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        {
            this->menubar();
              if (ImGui::CollapsingHeader("General report"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }

                if (ImGui::CollapsingHeader("Steps"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
                
                if (ImGui::CollapsingHeader("Metrics"))
                {
                    static int i0 = 123;
                    ImGui::InputInt("input int", &i0);

                    static float f0 = 0.001f;
                    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                    static double d0 = 999999.00000001;
                    ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                    static float f1 = 1.e10f;
                    ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
                }
        }
        ImGui::End();


        return "rendering";
        }
        else{

        return "closed";
    }
    }



void ReportInstance::menubar(){
            if (ImGui::BeginMenuBar())
            {
                std::string label = "Reporting task :\"" + this->task->tasktype + "\" with id : " + "\"" +this->task->id + "\"";
                ImGui::Text(label.c_str());
                ImGui::EndMenuBar();
            }


}

