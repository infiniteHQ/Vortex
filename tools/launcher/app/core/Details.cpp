
#include "Details.hpp"

enum class VortexEditor_Type
{
    VortexEditor_String_,
    VortexEditor_Integer_
};

struct VortexEditor_String
{

    void init(const std::string &value)
    {
        char *new_val = new char[256];
        strncpy(new_val, value.c_str(), 256);
        new_val[255] = '\0';

        this->m_Value = std::shared_ptr<char>(new_val, [](char *ptr)
                                              { delete[] ptr; });
    }

    std::shared_ptr<char> get()
    {
        return this->m_Value;
    }

    std::string get_name()
    {
        return this->m_Name;
    }

    VortexEditor_String(const std::string &name, const std::string &val) : m_Name(name)
    {
        this->init(val);
    }

    VortexEditor_String(const std::string &name) : m_Name(name)
    {
        //
    }

    std::string m_Name;
    std::shared_ptr<char> m_Value;
};

struct VortexEditor_Integer
{
    void init(const int &value)
    {
        this->m_Value = std::make_shared<int>(value);
    }

    std::shared_ptr<int> get()
    {
        return this->m_Value;
    }

    std::string get_name()
    {
        return this->m_Name;
    }

    VortexEditor_Integer(const std::string &name, int val) : m_Name(name)
    {
        this->init(val);
    }

    VortexEditor_Integer(const std::string &name) : m_Name(name)
    {
        //
    }

    std::string m_Name;
    std::shared_ptr<int> m_Value;
};

std::vector<VortexEditor_Integer> integers;
std::vector<VortexEditor_String> strings;

std::shared_ptr<int> get_integer(const std::string &name)
{
    for (auto integer : integers)
    {
        if (integer.get_name() == name)
        {
            return integer.get();
        }
    }

    return nullptr;
}
std::shared_ptr<char> get_string(const std::string &name)
{
    for (auto string : strings)
    {
        if (string.get_name() == name)
        {
            return string.get();
        }
    }

    return nullptr;
}

std::vector<std::pair<std::string, std::vector<std::pair<VortexEditor_Type, std::string>>>> itemMap;

void insert_item(const std::string &section, VortexEditor_Integer value)
{
    // Ajouter l'élément aux valeurs entières (supposant que "integers" est défini quelque part)
    integers.push_back(value);
    std::pair<VortexEditor_Type, std::string> new_item;
    new_item.first = VortexEditor_Type::VortexEditor_Integer_;
    new_item.second = value.get_name();

    // Utiliser une référence pour itérer à travers itemMap
    for (auto& pair : itemMap)
    {
        if (pair.first == section)
        {
            pair.second.push_back(new_item);
            return;
        }
    }

    // Si la section n'existe pas, ajouter une nouvelle section
    std::pair<std::string, std::vector<std::pair<VortexEditor_Type, std::string>>> new_section;
    new_section.first = section;
    new_section.second.push_back(new_item);
    itemMap.push_back(new_section);
}



void insert_item(const std::string &section, VortexEditor_String value)
{
    // Ajouter l'élément aux valeurs de chaînes (supposant que "strings" est défini quelque part)
    strings.push_back(value);
    std::pair<VortexEditor_Type, std::string> new_item;
    new_item.first = VortexEditor_Type::VortexEditor_String_;
    new_item.second = value.get_name();

    // Utiliser une référence pour itérer à travers itemMap
    for (auto& pair : itemMap)
    {
        if (pair.first == section)
        {
            pair.second.push_back(new_item);
            return;
        }
    }

    // Si la section n'existe pas, ajouter une nouvelle section
    std::pair<std::string, std::vector<std::pair<VortexEditor_Type, std::string>>> new_section;
    new_section.first = section;
    new_section.second.push_back(new_item);
    itemMap.push_back(new_section);
}


Details::Details(VxContext *_ctx, const std::string &_parent)
{
    this->ctx = _ctx;

    this->Refresh();

    parent = _parent;

    insert_item("main", VortexEditor_Integer("First int to", 2));
    insert_item("main", VortexEditor_Integer("Another integer__", 3));
    insert_item("main", VortexEditor_Integer("Another qsdinteger__", 3));
    insert_item("main", VortexEditor_Integer("Another inqsdteger lorem ipsum dolor si amet av toroter", 3));
    insert_item("main", VortexEditor_Integer("Another igfsdgnteger__", 3));
    insert_item("main", VortexEditor_Integer("Anothdfhfher integer__", 3));
    insert_item("second", VortexEditor_Integer("Another integer", 5));
    insert_item("second", VortexEditor_String("QSdQSDr", "fdsfs"));

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, parent, data);
        free(data);
    }
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

    for (auto section : itemMap)
    {
        if (ImGui::CollapsingHeader(section.first.c_str()))
        {
            ImGui::PopStyleVar(1);



static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableFlags_Borders | ImGuiTableFlags_BordersInnerV;

if (ImGui::BeginTable("tablhjke_", 2, flags))
{
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

    // Pushing style color for borders
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, IM_COL32(0, 0, 0, 255)); // Color for outer borders
    ImGui::PushStyleColor(ImGuiCol_TableBorderLight, IM_COL32(0, 0, 0, 255));  // Color for inner borders

    ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, IM_COL32(200, 200, 200, 255));

    std::cout << "f" << section.second.size() << std::endl;

    for (int row = 0; row < section.second.size(); row++)
    {
        ImGui::TableNextRow();
        for (int column = 0; column < 2; column++)
        {
            ImGui::TableSetColumnIndex(column);

            if (column == 0)
            {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 40));
                ImGui::AlignTextToFramePadding();
                ImGui::Indent(10.0f);
                ImGui::TextWrapped(section.second[row].second.c_str());
                ImGui::Unindent(10.0f);
            }
            else if (column == 1)
            {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 40));
                std::string label = "###Rest" + section.second[row].second;

                if (section.second[row].first == VortexEditor_Type::VortexEditor_Integer_)
                {
                    std::shared_ptr<int> value = get_integer(section.second[row].second);
                    if (value)
                    {
                        ImGui::SetNextItemWidth(-FLT_MIN);  // Set the item width to take the full width
                        ImGui::InputInt(label.c_str(), value.get());
                    }
                    else
                    {
                        ImGui::Text("INVALID VALUE");
                    }
                }
                else if (section.second[row].first == VortexEditor_Type::VortexEditor_String_)
                {
                    std::shared_ptr<char> value = get_string(section.second[row].second);
                    if (value)
                    {
                        ImGui::SetNextItemWidth(-FLT_MIN);  // Set the item width to take the full width
                        ImGui::InputText(label.c_str(), value.get(), sizeof(value.get()));
                    }
                    else
                    {
                        ImGui::Text("INVALID VALUE");
                    }
                }
            }
        }
    }

    ImGui::PopStyleColor(4); // Popping 4 style colors: 2 for borders and 2 for row backgrounds
    ImGui::PopStyleVar();
    ImGui::EndTable();
}



  }
    }

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
