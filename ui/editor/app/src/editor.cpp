#include "./editor.hpp"

static void handleExit(Cherry::Application *app)
{
    app->Close();
}

static void handleProjectSettings()
{
}

static void handleManagePlugins()
{
}

static void handleManageModules(bool &showModulesManager)
{
}

static void handleShowBottomToolbar()
{
}

static void handleLogUtility()
{
}

static void handleShowSimplifiedHeader(Cherry::Application *app)
{
    app->m_DefaultSpecification.CustomTitlebar = !app->m_DefaultSpecification.CustomTitlebar;
}

static void handleSpawnWindow(Cherry::Application *app)
{
    std::string dfqsf = "QSDff";
    app->SpawnWindow(dfqsf);
}

static void handleContentBrowser(bool &showContentBrowser)
{
}

static void handleProjectViewer(bool &showProjectViewer)
{
}

static void handleNews()
{
}

static void handleCommunity()
{
}

static void handleTutorials()
{
}

static void handleDocumentation()
{
}

void EditorLayer::framebar(const std::shared_ptr<EditorLayer> &applayer, Cherry::Application *app)
{
    float oldSize = ImGui::GetFont()->Scale;
    ImGui::PushFont(ImGui::GetFont());

    const char *text = VortexMaker::GetCurrentContext()->name.c_str();
    ImGui::GetFont()->Scale *= 0.84;
    ImGui::PushFont(ImGui::GetFont());

    ImVec2 textSize = ImGui::CalcTextSize(text);
    std::cout << "Name" << VortexMaker::GetCurrentContext()->name << std::endl;

    float circleRadius = 10.0f;
    float circlePadding = 12.0f;
    float rectanglePaddingX = 15.0f;
    float rectanglePaddingY = 25.0f;

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    float yOffset = 5.0f;

    ImVec2 circlePos = ImVec2(cursorPos.x - 12 + circleRadius, cursorPos.y + textSize.y * 0.1f + 6);

    ImVec2 rectMin = ImVec2(cursorPos.x + circleRadius + circlePadding, cursorPos.y - 38 + yOffset);
    ImVec2 rectMax = ImVec2(rectMin.x + textSize.x + 2 * rectanglePaddingX, cursorPos.y + textSize.y + 2 * rectanglePaddingY - 45);

    ImDrawList *drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(rectMin, rectMax, IM_COL32(15, 15, 15, 255));

    ImGui::SetCursorScreenPos(circlePos);
    ImGui::InvisibleButton("circleButton", ImVec2(circleRadius * 2, circleRadius * 2));

    if (ImGui::IsItemHovered())
    {
        drawList->AddCircle(circlePos, circleRadius + 1.0f, IM_COL32(200, 200, 200, 255), 32, 2.0f);
    }

    drawList->AddCircleFilled(circlePos, circleRadius, IM_COL32(40, 40, 40, 255));

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        ImGui::OpenPopup("circleMenu");
    }

    if (ImGui::BeginPopup("circleMenu"))
    {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Disconnected");
        if (ImGui::MenuItem("Logout"))
        {
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Connect to Vortex community"))
        {
        }
        ImGui::Separator();
        if (ImGui::MenuItem("See my marketplace contents"))
        {
        }
        if (ImGui::MenuItem("See my marketplace plugins/modules"))
        {
        }
        if (ImGui::MenuItem("See my marketplace templates"))
        {
        }
        if (ImGui::MenuItem("See my VortexHub projects"))
        {
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Upload content(s)"))
        {
        }
        ImGui::EndPopup();
    }

    ImVec2 textPos = ImVec2(rectMin.x + rectanglePaddingX, rectMin.y + (rectMax.y - rectMin.y - textSize.y) * 0.5f + rectanglePaddingY - 10);

    ImGui::SetCursorScreenPos(textPos);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        ImGui::OpenPopup("projectMenu");
    }

    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
    if (ImGui::BeginPopup("projectMenu"))
    {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
        if (ImGui::MenuItem("Project settings"))
        {
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Connect to Vortex community"))
        {
        }
        ImGui::Separator();
        if (ImGui::MenuItem("See my marketplace contents"))
        {
        }
        if (ImGui::MenuItem("See my marketplace plugins/modules"))
        {
        }
        if (ImGui::MenuItem("See my marketplace templates"))
        {
        }
        if (ImGui::MenuItem("See my VortexHub projects"))
        {
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Upload content(s)"))
        {
        }
        ImGui::EndPopup();
    }

    ImGui::GetFont()->Scale = oldSize;
    ImGui::PopFont();

    ImGui::PopFont();
}

void Editor::Menubar(const std::shared_ptr<EditorLayer> &exampleLayer, Cherry::Application *app)
{
    float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 0.84;
    ImGui::PushFont(ImGui::GetFont());

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Exit"))
            handleExit(app);
        ImGui::EndMenu();
    }

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);              // Gris (50% blanc)
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);     // Gris (50% blanc)
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // Fond plus foncé
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);       // Bordure plus claire

    // Pousser le style pour le fond plus foncé
    ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

    // Pousser le style pour la bordure plus claire
    ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    if (ImGui::BeginMenu("Edit"))
    {

        ImGui::GetFont()->Scale *= 0.8;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
        ImGui::Text("Main stuff");
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
        ImGui::Separator();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = 0.84;
        ImGui::PopFont();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

        if (ImGui::MenuItem("Project Settings", "Main configurations of this project", &exampleLayer->ShowProjectSettings))
            handleProjectSettings();
        ImGui::GetFont()->Scale *= 0.8;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
        ImGui::Text("Main stuff");
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
        ImGui::Separator();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = 0.84;
        ImGui::PopFont();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

        if (ImGui::MenuItem("Logs Utility", "Overview of all logs", &exampleLayer->ShowLogUtility))
            handleLogUtility();
        ImGui::GetFont()->Scale *= 0.8;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
        ImGui::Text("Main stuff");
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
        ImGui::Separator();
        ImGui::PopStyleColor();

        ImGui::GetFont()->Scale = 0.84;
        ImGui::PopFont();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

        if (ImGui::MenuItem("Manage plugins", "Add, remove, edit plugins of this project"))
            handleManagePlugins();
        if (ImGui::MenuItem("Manage modules", "Manage modules loaded/registered", &exampleLayer->ShowModulesManager))
            handleManageModules(exampleLayer->ShowModulesManager);
        if (ImGui::MenuItem("Templates modules", "Create, add template in your project", &exampleLayer->ShowTemplateManager))
            handleManageModules(exampleLayer->ShowTemplateManager);
        ImGui::EndMenu();
    }

    ImGui::PopStyleVar();    // Pour les bords arrondis
    ImGui::PopStyleColor(2); // Pour le fond et la bordure

    if (ImGui::BeginMenu("Window"))
    {
        if (ImGui::MenuItem("Show bottom toolbar", "Get some useful tools in a bottom bar."))
            handleShowBottomToolbar();
        if (ImGui::MenuItem("Show simplified header", "Reduce the size of header"))
            handleShowSimplifiedHeader(app);
        if (ImGui::MenuItem("SpawnNewWindow", "Reduce the size of header"))
            handleSpawnWindow(app);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools"))
    {
        if (ImGui::MenuItem("Content Browser", "Open a new project content browser", &exampleLayer->ShowContentBrowser))
        {
            this->SpawnContentBrowser();
        }
        if (ImGui::MenuItem("Project Viewer", "Project component manager", &exampleLayer->ShowProjectViewer))
            handleProjectViewer(exampleLayer->ShowProjectViewer);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help"))
    {

        if (ImGui::Button("About Vortex.."))
            ImGui::OpenPopup("About Vortex");
        if (ImGui::BeginPopupModal("About Vortex", NULL, ImGuiWindowFlags_MenuBar))
        {

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Close"))
                    {
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::Text("Vortex is complete open creation platform that contain a bunch of tools for creators and makers. ");

            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("News", "Get latest Vortex news"))
            handleNews();
        if (ImGui::MenuItem("Community", "Join a community of creators"))
            handleCommunity();
        if (ImGui::MenuItem("Tutorials", "Get bunch of tutorials"))
            handleTutorials();
        if (ImGui::MenuItem("Documentation", "See official documentation of Vortex Maker"))
            handleDocumentation();
        ImGui::Separator();
        if (ImGui::MenuItem("Exit"))
            handleExit(app);
        ImGui::EndMenu();
    }

    // Custom component menubar callback...

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();
}

Cherry::Application *CreateEditor(int argc, char **argv)
{
    Cherry::ApplicationSpecification spec;
    std::shared_ptr<EditorLayer> layer = std::make_shared<EditorLayer>();

    std::string name = "Vortex Editor";
    spec.Name = name;
    spec.MinHeight = 500;
    spec.MinWidth = 500;
    spec.Height = 900;
    spec.Width = 1600;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::DockingWindows;

    spec.DisableTitle = true;
    spec.WindowSaves = false;
    spec.IconPath = Cherry::Application::CookPath("resources/imgs/icon.png");

    Cherry::Application *app = new Cherry::Application(spec);
    app->SetFavIconPath(Cherry::Application::CookPath("resources/imgs/favicon.png"));
    app->AddFont("Consola", Cherry::Application::CookPath("resources/fonts/consola.ttf"), 17.0f);

    app->AddLocale("fr", Cherry::GetPath("resources/locales/fr.json"));
    app->AddLocale("en", Cherry::GetPath("resources/locales/en.json"));
    app->SetDefaultLocale("en");
    app->SetLocale("fr");

    app->PushLayer(layer);
    app->SetFramebarCallback([app, layer]()
                             {
        float oldSize = ImGui::GetFont()->Scale;
    ImGui::PushFont(ImGui::GetFont());

    const char *text = VortexMaker::GetCurrentContext()->name.c_str();
    ImGui::GetFont()->Scale *= 0.84;
    ImGui::PushFont(ImGui::GetFont());

    ImVec2 textSize = ImGui::CalcTextSize(text);

    float circleRadius = 10.0f;
    float circlePadding = 12.0f;
    float rectanglePaddingX = 15.0f;
    float rectanglePaddingY = 25.0f;

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    float yOffset = 5.0f;

    ImVec2 circlePos = ImVec2(cursorPos.x - 12 + circleRadius, cursorPos.y + textSize.y * 0.1f + 6);

    ImVec2 rectMin = ImVec2(cursorPos.x + circleRadius + circlePadding, cursorPos.y - 38 + yOffset);
    ImVec2 rectMax = ImVec2(rectMin.x + textSize.x + 2 * rectanglePaddingX, cursorPos.y + textSize.y + 2 * rectanglePaddingY - 45);

    ImDrawList *drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(rectMin, rectMax, IM_COL32(15, 15, 15, 255));

    ImGui::SetCursorScreenPos(circlePos);
    ImGui::InvisibleButton("circleButton", ImVec2(circleRadius * 2, circleRadius * 2));

    if (ImGui::IsItemHovered())
    {
        drawList->AddCircle(circlePos, circleRadius + 1.0f, IM_COL32(200, 200, 200, 255), 32, 2.0f);
    }

    drawList->AddCircleFilled(circlePos, circleRadius, IM_COL32(40, 40, 40, 255));

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        ImGui::OpenPopup("circleMenu");
    }

    if (ImGui::BeginPopup("circleMenu"))
    {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Disconnected");
        if (ImGui::MenuItem("Logout"))
        {
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Connect to Vortex community"))
        {
        }
        ImGui::Separator();
        if (ImGui::MenuItem("See my marketplace contents"))
        {
        }
        if (ImGui::MenuItem("See my marketplace plugins/modules"))
        {
        }
        if (ImGui::MenuItem("See my marketplace templates"))
        {
        }
        if (ImGui::MenuItem("See my VortexHub projects"))
        {
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Upload content(s)"))
        {
        }
        ImGui::EndPopup();
    }

    ImVec2 textPos = ImVec2(rectMin.x + rectanglePaddingX, rectMin.y + (rectMax.y - rectMin.y - textSize.y) * 0.5f + rectanglePaddingY - 10);

    ImGui::SetCursorScreenPos(textPos);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        ImGui::OpenPopup("projectMenu");
    }

    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
    if (ImGui::BeginPopup("projectMenu"))
    {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
        if (ImGui::MenuItem("Project settings"))
        {
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Connect to Vortex community"))
        {
        }
        ImGui::Separator();
        if (ImGui::MenuItem("See my marketplace contents"))
        {
        }
        if (ImGui::MenuItem("See my marketplace plugins/modules"))
        {
        }
        if (ImGui::MenuItem("See my marketplace templates"))
        {
        }
        if (ImGui::MenuItem("See my VortexHub projects"))
        {
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Upload content(s)"))
        {
        }
        ImGui::EndPopup();
    }

    ImGui::GetFont()->Scale = oldSize;
    ImGui::PopFont();

    ImGui::PopFont(); });

    app->SetMenubarCallback([app, layer]()
                            {
                            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);        
                            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
                            ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
                            ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);       

                            ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

                            ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

                            ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

                            static bool t;

                            if (ImGui::BeginMenu("Project"))
                            {
        if (ImGui::MenuItem("Modules utility", "Open the modules utility", c_Editor->GetModuleUtilityVisibility()))
        {
            c_Editor->SetModuleUtilityVisibility(true);
        }
        if (ImGui::MenuItem("Templates utility", "Open the templates utility", c_Editor->GetTemplatesUtilityVisibility()))
        {
            c_Editor->SetTemplatesUtilityVisibility(true);
        }
        if (ImGui::MenuItem("Project settings", "Open the main settings of this project", c_Editor->GetProjectSettingsVisibility()))
        {
            c_Editor->SetProjectSettingsVisibility(true);
        }
                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Edit"))
                            {
                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Window"))
                            {
                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Tools"))
                            {
        if (ImGui::MenuItem("Content Browser", "Open a new project content browser", false))
        {
            c_Editor->SpawnContentBrowser();
        }
        if (ImGui::MenuItem("Logs utility", "Open a new console logs", false))
        {
            c_Editor->SpawnLogsUtility();
        }
                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Options"))
                            {
                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Main stuff");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if(ImGui::Button("Set en"))
                              {
  app->SetLocale("en");
                              }

                              if(ImGui::Button("Set fr"))
                              {
  app->SetLocale("fr");
                              }

                              if(ImGui::Button("Set es"))
                              {
  app->SetLocale("es");
                              }
                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Main stuff");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem("Logs Utility", "Overview of all logs", &t))
                              {
                              }

                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Main stuff");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem("Manage plugins", "Add, remove, edit plugins of this project"))
                              {
                              }

                              if (ImGui::MenuItem("Manage modules", "Manage modules loaded/registered", &t))
                              {
                              }

                              if (ImGui::MenuItem("Templates modules", "Create, add template in your project", &t))
                              {
                              }

                              ImGui::EndMenu();
                            }


                            if (ImGui::BeginMenu("Help"))
                            {
                              ImGui::EndMenu();
                            }

                            // Callback
                            /*ImGui::Separator();

                            if (ImGui::BeginMenu("File"))
                            {
                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Select"))
                            {
                              ImGui::EndMenu();
                            }*/

                            ImGui::PopStyleVar();  
                            ImGui::PopStyleColor(2); }

    );

    c_Editor = std::make_shared<Editor>();
    return app;
}

int VortexMaker::VortexEditor(int argc, char **argv)
{
    return Cherry::ThirdMain(argc, argv, CreateEditor);
}