#include "crash_report.hpp"

static bool consent_data_inspection;
static bool consent_response;

CrashAppWindow::CrashAppWindow(const std::string &name)
{
    this->ctx = VortexMaker::GetCurrentContext();
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
    m_AppWindow->SetClosable(false);

    m_AppWindow->m_TabMenuCallback = []()
    {
        ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
        ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
            ImGui::Text("Main stuff");
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
            ImGui::Separator();
            ImGui::PopStyleColor();

            if (ImGui::MenuItem("Logs Utility", "Overview of all logs"))
            {
            }

            if (ImGui::MenuItem("Logs2 Utility", "Overview of all logs"))
            {
            }

            ImGui::EndMenu();
        }
    };

    m_AppWindow->SetInternalPaddingX(10.0f);
    m_AppWindow->SetInternalPaddingY(10.0f);

    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
}

// Updated loadFileToString function
bool loadFileToString(const std::string &filename, char *text, size_t maxSize)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false; // Unable to open file
    }

    std::string line;
    size_t index = 0;

    while (std::getline(file, line))
    {
        if (index + line.size() < maxSize - 1)
        {                                            // Check boundary to avoid overflow
            std::strcpy(text + index, line.c_str()); // Copy line into text buffer
            index += line.size();
            text[index++] = '\n'; // Add newline character
        }
        else
        {
            break; // Text buffer full
        }
    }

    text[index] = '\0'; // Null-terminate the string

    file.close();
    return true;
}

static std::vector<std::tuple<std::string, std::string, std::string>> modifiable_values;

std::shared_ptr<Cherry::AppWindow> &CrashAppWindow::GetAppWindow()
{
    return m_AppWindow;
}

std::shared_ptr<CrashAppWindow> CrashAppWindow::Create(const std::string &name)
{
    auto instance = std::shared_ptr<CrashAppWindow>(new CrashAppWindow(name));
    instance->SetupRenderCallback();
    return instance;
}

void CrashAppWindow::SetupRenderCallback()
{
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]()
                                   {
            if (self) {
                self->Render();
            } });
}

void CrashAppWindow::Render()
{
    float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.5;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::Text("Oups, Vortex crashed :(");

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 105));
    ImGui::TextWrapped("A Vortex process has encountered a fatal error or produced a bad output. This could be due to a faulty module, plugins, or a malfunction in the Vortex process itself. You can now review the last states and investigate what happened. However, it is safe to close this window.");
    ImGui::PopStyleColor(1);

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 50));
    ImGui::TextWrapped("Error topic ID : ");
    ImGui::PopStyleColor(1);

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 105));
    ImGui::TextWrapped(ctx->state.session_id.c_str());
    ImGui::PopStyleColor(1);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Summary"))
        {
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.3;
            ImGui::PushFont(ImGui::GetFont());
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 100));
            ImGui::TextWrapped("Latest informations");
            ImGui::PopStyleColor(1);
            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (ImGui::BeginTable("table_deps", 2, flags))
            {
                ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (int i = 0; i < 4; i++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (i == 0)
                        {
                            if (column == 0)
                            {
                                ImGui::Text("Latest fatal error");
                            }
                            else if (column == 1)
                            {
                                ImGui::Text("The current path doesn't exist !");
                            }
                        }
                        else if (i == 1)
                        {
                            if (column == 0)
                            {
                                ImGui::Text("Latest error");
                            }
                            else if (column == 1)
                            {
                                ImGui::Text("My big project");
                            }
                        }
                        else if (i == 2)
                        {
                            if (column == 0)
                            {
                                ImGui::Text("Latest warning");
                            }
                            else if (column == 1)
                            {
                                ImGui::Text("4qsfdsgsd");
                            }
                        }
                        else if (i == 3)
                        {
                            if (column == 0)
                            {
                                ImGui::Text("Latest information");
                            }
                            else if (column == 1)
                            {
                                ImGui::Text("4fhdgfhdfgg");
                            }
                        }
                    }
                }
                ImGui::EndTable();
            }

            ImGui::GetFont()->Scale *= 1.3;
            ImGui::PushFont(ImGui::GetFont());
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 100));
            ImGui::TextWrapped("Master context");
            ImGui::PopStyleColor(1);
            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            if (ImGui::BeginTable("table_deps", 2, flags))
            {
                ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
                for (int i = 0; i < 8; i++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (i == 0)
                        {
                            if (column == 0)
                            {
                                ImGui::Text("Vortex Version");
                            }
                            else if (column == 1)
                            {
                                ImGui::Text("1.1");
                            }
                        }
                        else if (i == 1)
                        {
                            if (column == 0)
                            {
                                ImGui::Text("Project name");
                            }
                            else if (column == 1)
                            {
                                ImGui::Text("My big project");
                            }
                        }
                        else if (i == 2)
                        {
                            if (column == 0)
                            {
                                ImGui::Text("Number of modules loaded");
                            }
                            else if (column == 1)
                            {
                                ImGui::Text("4");
                            }
                        }
                        else if (i == 3)
                        {
                            if (column == 0)
                            {
                                ImGui::Text("Number of plugins loaded");
                            }
                            else if (column == 1)
                            {
                                ImGui::Text("4");
                            }
                        }
                    }
                }
                ImGui::EndTable();
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Informations"))
        {
            static const int bufferSize = 1024 * 16;
            // Declare the text buffer
            static char text[bufferSize];

            // Load content from file into text variable
            static std::string log_file = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + ctx->state.session_id + "/logs/global.log";
            if (!loadFileToString(log_file, text, bufferSize))
            {
                // Handle error opening or reading the file
                ImGui::Text("Failed to open or read file: %s", log_file.c_str());
            }

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly;
            ImGui::InputTextMultiline("##logs_source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Processus report"))
        {

            static const int bufferSize = 1024 * 16;
            // Declare the text buffer
            static char text[bufferSize];

            // Load content from file into text variable
            static std::string log_file = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + ctx->state.session_id + "/crash/core_dumped.txt";
            if (!loadFileToString(log_file, text, bufferSize))
            {
                // Handle error opening or reading the file
                ImGui::Text("Failed to open or read file: %s", log_file.c_str());
            }

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly;
            ImGui::InputTextMultiline("##processus_source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Send details"))
        {
            ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::Checkbox("I consent send to Infinite data about my hardware & OS technical infos. ", &consent_data_inspection);
    ImGui::Checkbox("I consent Infinite can contact me to get more informations. ", &consent_response);

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImVec2 buttonSize = ImVec2(170, 30);
    ImVec2 bitButtonSize = ImVec2(300, 30);

    float ysection = windowSize.y - 280;
    float buttonPosY = windowSize.y - buttonSize.y - ImGui::GetStyle().ItemSpacing.y * 2 - bitButtonSize.y;

    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), buttonPosY));

    if (ImGui::Button("Don't send and close", buttonSize))
    {
    }

    float firstButtonPosX = windowSize.x - buttonSize.x - bitButtonSize.y - 80 * 2 - ImGui::GetStyle().ItemSpacing.x * 3;

    ImGui::SetCursorPos(ImVec2(firstButtonPosX, buttonPosY));

    if (ImGui::Button("Send and close", buttonSize))
    {
    }

    ImGui::SameLine();

    if (ImGui::Button("Send and restart", buttonSize))
    {
    }
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

void CrashAppWindow::addModuleModal()
{
}

void CrashAppWindow::mainButtonsMenuItem()
{
}

void CrashAppWindow::filterMenuItem()
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

void CrashAppWindow::createMenuItem()
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

void CrashAppWindow::searchMenuItem()
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

void CrashAppWindow::menubar()
{
    if (ImGui::BeginMenuBar())
    {
        this->mainButtonsMenuItem();
        ImGui::EndMenuBar();
    }
}

void CrashAppWindow::Refresh() {
};
