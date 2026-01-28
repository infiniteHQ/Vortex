#include "./editor.hpp"

std::string GetVortexBuildType() { return VORTEX_BUILD; }

static void handleExit(Cherry::Application *app) { app->Close(); }

static void handleProjectSettings() {}

static void handleManagePlugins() {}

static void handleManageModules(bool &showModulesManager) {}

static void handleShowBottomToolbar() {}

static void handleLogUtility() {}

static void handleShowSimplifiedHeader(Cherry::Application *app) {
  app->m_DefaultSpecification.CustomTitlebar =
      !app->m_DefaultSpecification.CustomTitlebar;
}

static void handleSpawnWindow(Cherry::Application *app) {
  std::string dfqsf = "QSDff";
  app->SpawnWindow(dfqsf);
}

static void handleContentBrowser(bool &showContentBrowser) {}

static void handleProjectViewer(bool &showProjectViewer) {}

static void handleNews() {}

static void handleCommunity() {}

static void handleTutorials() {}

static void handleDocumentation() {}
/*
void EditorLayer::framebar(const std::shared_ptr<EditorLayer> &applayer,
                           Cherry::Application *app) {
  float oldSize = CherryGUI::GetFont()->Scale;
  CherryGUI::PushFont(CherryGUI::GetFont());

  const char *text = VortexMaker::GetCurrentContext()->name.c_str();
  CherryGUI::GetFont()->Scale *= 0.84;
  CherryGUI::PushFont(CherryGUI::GetFont());

  ImVec2 textSize = CherryGUI::CalcTextSize(text);

  float circleRadius = 10.0f;
  float circlePadding = 12.0f;
  float rectanglePaddingX = 15.0f;
  float rectanglePaddingY = 25.0f;

  ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

  float yOffset = 5.0f;

  ImVec2 circlePos = ImVec2(cursorPos.x - 12 + circleRadius,
                            cursorPos.y + textSize.y * 0.1f + 6);

  ImVec2 rectMin = ImVec2(cursorPos.x + circleRadius + circlePadding,
                          cursorPos.y - 38 + yOffset);
  ImVec2 rectMax =
      ImVec2(rectMin.x + textSize.x + 2 * rectanglePaddingX,
             cursorPos.y + textSize.y + 2 * rectanglePaddingY - 45);

  ImDrawList *drawList = CherryGUI::GetWindowDrawList();
  drawList->AddRectFilled(rectMin, rectMax, IM_COL32(15, 15, 15, 255));

  CherryGUI::SetCursorScreenPos(circlePos);
  CherryGUI::InvisibleButton("circleButton",
                             ImVec2(circleRadius * 2, circleRadius * 2));

  if (CherryGUI::IsItemHovered()) {
    drawList->AddCircle(circlePos, circleRadius + 1.0f,
                        IM_COL32(200, 200, 200, 255), 32, 2.0f);
  }

  drawList->AddCircleFilled(circlePos, circleRadius, IM_COL32(40, 40, 40, 255));

  if (CherryGUI::IsItemClicked(ImGuiMouseButton_Left)) {
    CherryGUI::OpenPopup("circleMenu");
  }

  if (CherryGUI::BeginPopup("circleMenu")) {
    CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Disconnected");
    if (CherryGUI::MenuItem("Logout")) {
    }

    CherryGUI::Separator();
    if (CherryGUI::MenuItem("Connect to Vortex community")) {
    }
    CherryGUI::Separator();
    if (CherryGUI::MenuItem("See my marketplace contents")) {
    }
    if (CherryGUI::MenuItem("See my marketplace plugins/modules")) {
    }
    if (CherryGUI::MenuItem("See my marketplace templates")) {
    }
    if (CherryGUI::MenuItem("See my VortexHub projects")) {
    }
    CherryGUI::Separator();
    if (CherryGUI::MenuItem("Upload content(s)")) {
    }
    CherryGUI::EndPopup();
  }

  ImVec2 textPos =
      ImVec2(rectMin.x + rectanglePaddingX,
             rectMin.y + (rectMax.y - rectMin.y - textSize.y) * 0.5f +
                 rectanglePaddingY - 10);

  CherryGUI::SetCursorScreenPos(textPos);

  if (CherryGUI::IsItemClicked(ImGuiMouseButton_Left)) {
    CherryGUI::OpenPopup("projectMenu");
  }

  CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
  if (CherryGUI::BeginPopup("projectMenu")) {
    CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
    if (CherryGUI::MenuItem("Project settings")) {
    }

    CherryGUI::Separator();
    if (CherryGUI::MenuItem("Connect to Vortex community")) {
    }
    CherryGUI::Separator();
    if (CherryGUI::MenuItem("See my marketplace contents")) {
    }
    if (CherryGUI::MenuItem("See my marketplace plugins/modules")) {
    }
    if (CherryGUI::MenuItem("See my marketplace templates")) {
    }
    if (CherryGUI::MenuItem("See my VortexHub projects")) {
    }
    CherryGUI::Separator();
    if (CherryGUI::MenuItem("Upload content(s)")) {
    }
    CherryGUI::EndPopup();
  }

  CherryGUI::GetFont()->Scale = oldSize;
  CherryGUI::PopFont();

  CherryGUI::PopFont();
}
*/
void Editor::Menubar(Cherry::Application *app) {
  float oldsize = CherryGUI::GetFont()->Scale;
  CherryGUI::GetFont()->Scale *= 0.84;
  CherryGUI::PushFont(CherryGUI::GetFont());

  if (CherryGUI::BeginMenu("File")) {
    if (CherryGUI::MenuItem("Exit"))
      handleExit(app);
    CherryGUI::EndMenu();
  }

  ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
  ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
  ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

  CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
  CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

  CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

  if (CherryGUI::BeginMenu("Edit")) {
    // TODO : Save All, (like the content browser)
    CherryGUI::GetFont()->Scale *= 0.8;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

    CherryGUI::PushStyleColor(ImGuiCol_Text, grayColor);
    CherryGUI::Text("Main stuff");
    CherryGUI::PopStyleColor();

    CherryGUI::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
    CherryGUI::Separator();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = 0.84;
    CherryGUI::PopFont();
    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

    if (CherryGUI::MenuItem("Project Settings",
                            "Main configurations of this project",
                            &ShowProjectSettings))
      handleProjectSettings();
    CherryGUI::GetFont()->Scale *= 0.8;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

    CherryGUI::PushStyleColor(ImGuiCol_Text, grayColor);
    CherryGUI::Text("Main stuff");
    CherryGUI::PopStyleColor();

    CherryGUI::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
    CherryGUI::Separator();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = 0.84;
    CherryGUI::PopFont();
    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

    if (CherryGUI::MenuItem("Logs Utility", "Overview of all logs",
                            &ShowLogUtility))
      handleLogUtility();
    CherryGUI::GetFont()->Scale *= 0.8;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

    CherryGUI::PushStyleColor(ImGuiCol_Text, grayColor);
    CherryGUI::Text("Main stuff");
    CherryGUI::PopStyleColor();

    CherryGUI::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
    CherryGUI::Separator();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = 0.84;
    CherryGUI::PopFont();
    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

    if (CherryGUI::MenuItem("Manage plugins",
                            "Add, remove, edit plugins of this project"))
      handleManagePlugins();
    if (CherryGUI::MenuItem("Manage modules",
                            "Manage modules loaded/registered",
                            &ShowModulesManager))
      handleManageModules(ShowModulesManager);
    if (CherryGUI::MenuItem("Templates modules",
                            "Create, add template in your project",
                            &ShowTemplateManager))
      handleManageModules(ShowTemplateManager);
    CherryGUI::EndMenu();
  }

  CherryGUI::PopStyleVar();
  CherryGUI::PopStyleColor(2);

  if (CherryGUI::BeginMenu("Window")) {
    if (CherryGUI::MenuItem("Show bottom toolbar",
                            "Get some useful tools in a bottom bar."))
      handleShowBottomToolbar();
    if (CherryGUI::MenuItem("Show simplified header",
                            "Reduce the size of header"))
      handleShowSimplifiedHeader(app);
    if (CherryGUI::MenuItem("SpawnNewWindow", "Reduce the size of header"))
      handleSpawnWindow(app);
    CherryGUI::EndMenu();
  }

  if (CherryGUI::BeginMenu("Tools")) {
    if (CherryGUI::MenuItem("Content Browser",
                            "Open a new project content browser",
                            &ShowContentBrowser)) {
      this->SpawnContentBrowser();
    }
    if (CherryGUI::MenuItem("Project Viewer", "Project component manager",
                            &ShowProjectViewer))
      handleProjectViewer(ShowProjectViewer);
    CherryGUI::EndMenu();
  }

  if (CherryGUI::BeginMenu("Help")) {

    if (CherryGUI::Button("About Vortex.."))
      CherryGUI::OpenPopup("About Vortex");
    if (CherryGUI::BeginPopupModal("About Vortex", NULL,
                                   ImGuiWindowFlags_MenuBar)) {

      if (CherryGUI::BeginMenuBar()) {
        if (CherryGUI::BeginMenu("File")) {
          if (CherryGUI::MenuItem("Close")) {
          }
          CherryGUI::EndMenu();
        }
        CherryGUI::EndMenuBar();
      }
      CherryGUI::Text(
          "Vortex is complete open creation platform that contain a "
          "bunch of tools for creators and makers. ");

      if (CherryGUI::Button("Close"))
        CherryGUI::CloseCurrentPopup();
      CherryGUI::EndPopup();
    }
    CherryGUI::Separator();
    if (CherryGUI::MenuItem("News", "Get latest Vortex news"))
      handleNews();
    if (CherryGUI::MenuItem("Community", "Join a community of creators"))
      handleCommunity();
    if (CherryGUI::MenuItem("Tutorials", "Get bunch of tutorials"))
      handleTutorials();
    if (CherryGUI::MenuItem("Documentation",
                            "See official documentation of Vortex Maker"))
      handleDocumentation();
    CherryGUI::Separator();
    if (CherryGUI::MenuItem("Exit"))
      handleExit(app);
    CherryGUI::EndMenu();
  }

  // Custom component menubar callback...

  CherryGUI::GetFont()->Scale = oldsize;
  CherryGUI::PopFont();
}

// Frame task
void RebuildCherryTheme() {
  if (VortexMaker::IsThemeNeedsRebuild()) {
    for (auto t : VortexMaker::GetCurrentContext()->IO.themes) {
      if (!t) {
        continue;
      }

      auto theme = Cherry::Theme();
      theme.SetName(t->label);
      for (auto theme_prop : t->theme) {
        theme.SetProperty(theme_prop.first, theme_prop.second);
      }

      CherryApp.AddTheme(theme);
    }

    CherryApp.m_Themes.clear();

    // set selected theme
    auto selected_theme = VortexMaker::GetSelectedTheme();
    if (selected_theme) {

      CherryApp.SetTheme(selected_theme->label);
    }

    // set override themes
    VortexMaker::ThemeRebuilded();
  }
}

Cherry::Application *CreateEditor(int argc, char **argv) {
  Cherry::ApplicationSpecification spec;

  std::string name = "Vortex Editor";
  spec.Name = name;
  spec.MinHeight = 500;
  spec.MinWidth = 500;
  spec.Height = 900;
  spec.Width = 1800;
  spec.CustomTitlebar = true;
  spec.DisableWindowManagerTitleBar = true;
  spec.RenderMode = Cherry::WindowRenderingMethod::DockingWindows;

  spec.DisableTitle = true;
  spec.WindowSaves = false;
  spec.IconPath = Cherry::Application::CookPath("resources/imgs/icon.png");
  spec.FavIconPath = Cherry::Application::CookPath("resources/imgs/icon.png");

  spec.SetFramebarCallback([]() {
    float oldSize = CherryGUI::GetFont()->Scale;
    CherryGUI::PushFont(CherryGUI::GetFont());

    const char *text = VortexMaker::GetCurrentContext()->name.c_str();
    CherryGUI::GetFont()->Scale *= 0.84;
    CherryGUI::PushFont(CherryGUI::GetFont());

    ImVec2 textSize = CherryGUI::CalcTextSize(text);

    float circleRadius = 10.0f;
    float circlePadding = 12.0f;
    float rectanglePaddingX = 15.0f;
    float rectanglePaddingY = 25.0f;

    ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

    float yOffset = 5.0f;

    ImVec2 circlePos = ImVec2(cursorPos.x - 12 + circleRadius,
                              cursorPos.y + textSize.y * 0.1f + 6);

    ImVec2 rectMin = ImVec2(cursorPos.x + circleRadius + circlePadding,
                            cursorPos.y - 38 + yOffset);
    ImVec2 rectMax =
        ImVec2(rectMin.x + textSize.x + 2 * rectanglePaddingX,
               cursorPos.y + textSize.y + 2 * rectanglePaddingY - 45);

    ImDrawList *drawList = CherryGUI::GetWindowDrawList();
    drawList->AddRectFilled(rectMin, rectMax, IM_COL32(15, 15, 15, 255));

    if (GetVortexBuildType() == "dev") {
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() -
                               CherryGUI::CalcTextSize("DEV").x - 20.0f);

      CherryGUI::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "DEV");
    }

    CherryGUI::SetCursorScreenPos(circlePos);
    CherryGUI::InvisibleButton("circleButton",
                               ImVec2(circleRadius * 2, circleRadius * 2));

    if (CherryGUI::IsItemHovered()) {
      drawList->AddCircle(circlePos, circleRadius + 1.0f,
                          IM_COL32(200, 200, 200, 255), 32, 2.0f);
    }

    drawList->AddCircleFilled(circlePos, circleRadius,
                              IM_COL32(40, 40, 40, 255));

    if (CherryGUI::IsItemClicked(ImGuiMouseButton_Left)) {
      CherryGUI::OpenPopup("circleMenu");
    }

    if (CherryGUI::BeginPopup("circleMenu")) {
      CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Disconnected");
      if (CherryGUI::MenuItem("Logout")) {
      }

      CherryGUI::Separator();
      if (CherryGUI::MenuItem("Connect to Vortex community")) {
      }
      CherryGUI::Separator();
      if (CherryGUI::MenuItem("See my marketplace contents")) {
      }
      if (CherryGUI::MenuItem("See my marketplace plugins/modules")) {
      }
      if (CherryGUI::MenuItem("See my marketplace templates")) {
      }
      if (CherryGUI::MenuItem("See my VortexHub projects")) {
      }
      CherryGUI::Separator();
      if (CherryGUI::MenuItem("Upload content(s)")) {
      }
      CherryGUI::EndPopup();
    }

    ImVec2 textPos =
        ImVec2(rectMin.x + rectanglePaddingX,
               rectMin.y + (rectMax.y - rectMin.y - textSize.y) * 0.5f +
                   rectanglePaddingY - 10);

    CherryGUI::SetCursorScreenPos(textPos);

    if (CherryGUI::IsItemClicked(ImGuiMouseButton_Left)) {
      CherryGUI::OpenPopup("projectMenu");
    }

    CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
    if (CherryGUI::BeginPopup("projectMenu")) {
      CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
      if (CherryGUI::MenuItem("Project settings")) {
      }

      CherryGUI::Separator();
      if (CherryGUI::MenuItem("Connect to Vortex community")) {
      }
      CherryGUI::Separator();
      if (CherryGUI::MenuItem("See my marketplace contents")) {
      }
      if (CherryGUI::MenuItem("See my marketplace plugins/modules")) {
      }
      if (CherryGUI::MenuItem("See my marketplace templates")) {
      }
      if (CherryGUI::MenuItem("See my VortexHub projects")) {
      }
      CherryGUI::Separator();
      if (CherryGUI::MenuItem("Upload content(s)")) {
      }
      CherryGUI::EndPopup();
    }

    CherryGUI::GetFont()->Scale = oldSize;
    CherryGUI::PopFont();

    CherryGUI::PopFont();
  });
  Cherry::Application *app = new Cherry::Application(spec);
  app->SetFavIconPath(Cherry::Application::CookPath("resources/imgs/icon.png"));
  app->AddFont("Consola",
               Cherry::Application::CookPath("resources/fonts/consola.ttf"),
               17.0f);

  app->AddLocale("fr", Cherry::GetPath("resources/locales/fr.json"));
  app->AddLocale("en", Cherry::GetPath("resources/locales/en.json"));
  app->SetDefaultLocale("en");
  app->SetLocale("en");

  for (auto &modules : VortexMaker::GetCurrentContext()->IO.em) {
    modules->RefreshMainWindow();
  }

  app->SetMenubarCallback([app]() {
    if (VortexMaker::IsThemeNeedsRebuild()) {
      CherryApp.m_Themes.clear();
      for (auto t : VortexMaker::GetCurrentContext()->IO.themes) {
        if (!t) {
          continue;
        }

        auto theme = Cherry::Theme();
        theme.SetName(t->label);
        for (auto theme_prop : t->theme) {
          theme.SetProperty(theme_prop.first, theme_prop.second);
        }

        CherryApp.AddTheme(theme);
      }

      // set selected theme
      auto selected_theme = VortexMaker::GetSelectedTheme();
      if (selected_theme) {

        CherryApp.SetTheme(selected_theme->label);
      }

      // set override themes
      VortexMaker::ThemeRebuilded();
    }

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    static bool t;

    if (CherryGUI::BeginMenu("Project")) {
      CherryKit::SeparatorText("Logical contents");

      if (CherryGUI::MenuItem(
              "Modules utility", "Open the modules utility",
              Cherry::GetTexture(Cherry::GetPath(
                  "resources/imgs/icons/misc/icon_bricksearch.png")),
              c_Editor->GetModulesUtilityVisibility())) {
        c_Editor->SetModulesUtilityVisibility(
            !c_Editor->GetModulesUtilityVisibility());
      }
      if (CherryGUI::MenuItem("Plugins utility", "Open the plugins utility",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_plugin.png")),
                              c_Editor->GetModulesUtilityVisibility())) {
      }

      CherryKit::SeparatorText("Static contents");
      if (CherryGUI::MenuItem("Templates utility", "Open the templates utility",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_stack.png")),
                              c_Editor->GetTemplatesUtilityVisibility())) {
        c_Editor->SetTemplatesUtilityVisibility(
            !c_Editor->GetTemplatesUtilityVisibility());
      }
      if (CherryGUI::MenuItem("Contents utility", "Open the contents utility",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_stack.png")),
                              c_Editor->GetTemplatesUtilityVisibility())) {
      }

      CherryKit::SeparatorText("Configurations & Settings");
      if (CherryGUI::MenuItem(
              "Project settings", "Open the main settings of this project",
              Cherry::GetTexture(Cherry::GetPath(
                  "resources/imgs/icons/misc/icon_settings.png")),
              c_Editor->GetProjectSettingsVisibility())) {
        c_Editor->SetProjectSettingsVisibility(true);
      }

      CherryKit::SeparatorText("Other");

      if (CherryGUI::MenuItem("About Vortex", "About the Vortex Editor",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_info.png")),
                              c_Editor->GetAboutAppWindowVisibility())) {
        c_Editor->SetAboutWindowVisibility(
            !c_Editor->GetAboutAppWindowVisibility());
      }

      if (CherryGUI::MenuItem("About the Project", "About the current project",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_info.png")),
                              c_Editor->GetAboutAppWindowVisibility())) {
        c_Editor->SetAboutWindowVisibility(
            !c_Editor->GetAboutAppWindowVisibility());
      }

      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu("Edit")) {
      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu("Window")) {
      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu("Tools")) {

      CherryKit::SeparatorText("Contents, managment");

      if (CherryGUI::MenuItem(
              "Content Browser", "Open a content browser",
              Cherry::GetTexture(Cherry::GetPath(
                  "resources/imgs/icons/misc/icon_collection.png")),
              false)) {
        c_Editor->SpawnContentBrowser();
      }

      CherryKit::SeparatorText("Console, logs, debugging");
      if (CherryGUI::MenuItem(
              "Console logs", "Open a console with logs",
              Cherry::GetTexture(Cherry::GetPath(
                  "resources/imgs/icons/misc/icon_journal.png")),
              false)) {
        c_Editor->SpawnLogsUtility();
      }

      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu("Options")) {
      CherryGUI::GetFont()->Scale *= 0.8;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

      CherryGUI::PushStyleColor(ImGuiCol_Text, grayColor);
      CherryGUI::Text("Main stuff");
      CherryGUI::PopStyleColor();

      CherryGUI::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
      CherryGUI::Separator();
      CherryGUI::PopStyleColor();

      CherryGUI::GetFont()->Scale = 0.84;
      CherryGUI::PopFont();
      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (CherryGUI::Button("Set en")) {
        app->SetLocale("en");
      }

      if (CherryGUI::Button("Set fr")) {
        app->SetLocale("fr");
      }

      if (CherryGUI::Button("Set es")) {
        app->SetLocale("es");
      }
      CherryGUI::GetFont()->Scale *= 0.8;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

      CherryGUI::PushStyleColor(ImGuiCol_Text, grayColor);
      CherryGUI::Text("Main stuff");
      CherryGUI::PopStyleColor();

      CherryGUI::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
      CherryGUI::Separator();
      CherryGUI::PopStyleColor();

      CherryGUI::GetFont()->Scale = 0.84;
      CherryGUI::PopFont();
      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (CherryGUI::MenuItem("Logs Utility", "Overview of all logs", &t)) {
      }

      CherryGUI::GetFont()->Scale *= 0.8;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

      CherryGUI::PushStyleColor(ImGuiCol_Text, grayColor);
      CherryGUI::Text("Main stuff");
      CherryGUI::PopStyleColor();

      CherryGUI::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
      CherryGUI::Separator();
      CherryGUI::PopStyleColor();

      CherryGUI::GetFont()->Scale = 0.84;
      CherryGUI::PopFont();
      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (CherryGUI::MenuItem("Manage plugins",
                              "Add, remove, edit plugins of this project")) {
      }

      if (CherryGUI::MenuItem("Manage modules",
                              "Manage modules loaded/registered", &t)) {
      }

      if (CherryGUI::MenuItem("Templates modules",
                              "Create, add template in your project", &t)) {
      }

      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu("Help")) {
      CherryGUI::EndMenu();
    }

    // Callback
    /*CherryGUI::Separator();

    if (CherryGUI::BeginMenu("File"))
    {
      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu("Select"))
    {
      CherryGUI::EndMenu();
    }*/

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor(2);
  }

  );

  c_Editor = std::make_shared<Editor>();
  return app;
}

int VortexMaker::VortexEditor(int argc, char **argv) {
  return Cherry::ThirdMain(argc, argv, CreateEditor);
}