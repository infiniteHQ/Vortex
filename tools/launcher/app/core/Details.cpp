
#include "Details.hpp"

Details::Details(VxContext *_ctx, const std::string &_parent)
{
    std::cout << "df" << std::endl;
    this->ctx = _ctx;

    std::cout << "df" << std::endl;
    this->Refresh();

    std::cout << "df" << std::endl;
    parent = _parent;

    std::cout << "df" << std::endl;
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    std::cout << "d" << std::endl;
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_project, icons::i_project_size, w, h);
        m_ProjectIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_open, icons::i_open_size, w, h);
        m_OpenIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
    std::cout << "8" << std::endl;
}

static std::vector<std::tuple<std::string, std::string, std::string>> modifiable_values;

void Details::OnImGuiRender(const std::string &parent, std::function<void(ImGuiWindow *)> controller)
{

    static ImTextureID projectIcon = this->m_ProjectIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

    ImGui::SetNextWindowDockID(ImGui::GetID("MainDockspace"), ImGuiCond_FirstUseEver);
    if (ImGui::UIKit_BeginLogoWindow("Detail", &projectIcon, &this->opened, window_flags))
        this->menubar();
    static ImGuiWindow *win = ImGui::GetCurrentWindow();
    this->parent = parent;
    controller(win);

    static char tochanbgf[128];

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, 3.0f);
    if (ImGui::CollapsingHeader("Toolchain informations"))
    {
        ImGui::PopStyleVar(1);
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableColumnFlags_NoHeaderLabel;

        if (ImGui::BeginTable("tablhjke_", 2, flags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

            ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(200, 200, 200, 255));
            ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, IM_COL32(200, 200, 200, 255));

            for (int row = 0; row < 4; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 2; column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    if (column == 0)
                    {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 50));
                        ImGui::AlignTextToFramePadding();
                        ImGui::Indent(10.0f);
                        if (row == 0)
                        {
                            ImGui::Text("Toolchain Name");
                        }
                        else if (row == 1)
                        {
                            ImGui::Text("Always Use it");
                        }
                        else if (row == 2)
                        {
                            ImGui::Text("Toolchain Description");
                        }
                        else if (row == 3)
                        {
                            ImGui::Text("Toolchain Version");
                        }
                        else if (row == 4)
                        {
                            ImGui::Text("Toolchain Author");
                        }
                        ImGui::Unindent(10.0f);
                    }
                    else if (column == 1)
                    {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 50));
                        static char tochanbgf[128] = "";
                        std::string label = "###Rest" + row;
                        ImGui::InputText(label.c_str(), tochanbgf, 128);
                    }
                    ImGui::Separator();
                }
            }

            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Toolchain configurations"))
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableColumnFlags_NoHeaderLabel;

        if (ImGui::BeginTable("tablhjke_", 2, flags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

            ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(200, 200, 200, 255));
            ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, IM_COL32(200, 200, 200, 255));

            for (int row = 0; row < 3; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 2; column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    if (column == 0)
                    {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 50));
                        ImGui::AlignTextToFramePadding();
                        ImGui::Indent(10.0f);
                        if (row == 0)
                        {
                            ImGui::Text("Build architecture");
                        }
                        else if (row == 1)
                        {
                            ImGui::Text("Host architecture");
                        }
                        else if (row == 2)
                        {
                            ImGui::Text("Target architecture");
                        }
                        ImGui::Unindent(10.0f);
                    }
                    else if (column == 1)
                    {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 50));
                        static char tochanbgf[128] = "";
                        std::string label = "###Rest" + row;
                        ImGui::InputText(label.c_str(), tochanbgf, 128);
                    }
                    ImGui::Separator();
                }
            }

            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
            ImGui::EndTable();
        }
    }

    ImGui::End();
    std::cout << "222" << std::endl;
}

static void handleRefresh()
{
    // Behavior
}

static void handleAddToProject(const std::string &name, const std::string &version)
{
    // Behavior
}

static void handleFilterBuildRebuild()
{
    // Behavior
}

static void handleGlobalBuild()
{
    // Behavior
}

static void handleCreateModule()
{
    // Behavior
}

static void handleSearch()
{
    // Behavior
}

void Details::addModuleModal()
{
}

void Details::mainButtonsMenuItem()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Details::filterMenuItem()
{
    ImGui::Separator();
    if (ImGui::BeginMenu("Filters"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleFilterBuildRebuild();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void Details::createMenuItem()
{
    if (ImGui::BeginMenu("Create a module"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleCreateModule();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void Details::searchMenuItem()
{
    if (ImGui::BeginMenu("Search"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleSearch();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void Details::menubar()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::BeginMenuBar())
    {
        this->mainButtonsMenuItem();
        ImGui::EndMenuBar();
    }
}

void Details::Refresh()
{
    project_templates.clear();
    for (auto tem : this->ctx->IO.sys_templates)
    {
        if (tem->m_type == "project")
        {
            project_templates.push_back(tem);
        }
    }
};
