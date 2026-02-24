#include "./editor.hpp"

#include "../../../../vxcore/include/vortex.h"
#include "../../../../vxcore/include/vortex_internals.h"

std::string GetVortexBuildType() { return VORTEX_BUILD; }

Editor::Editor() {
  m_WelcomeAppWindow =
      VortexEditor::Welcome::Create("?loc:loc.window_names.welcome");
  m_WelcomeAppWindow->GetAppWindow()->SetVisibility(true);
  Cherry::AddAppWindow(m_WelcomeAppWindow->GetAppWindow());

  m_ModulesUtility = VortexEditor::ModulesUtility::Create("Modules utily");
  m_ModulesUtility->GetAppWindow()->SetVisibility(false);
  Cherry::AddAppWindow(m_ModulesUtility->GetAppWindow());

  m_PluginsUtility = VortexEditor::PluginsUtility::Create("Plugins utily");
  m_PluginsUtility->GetAppWindow()->SetVisibility(false);
  Cherry::AddAppWindow(m_PluginsUtility->GetAppWindow());

  m_ProjectSettings = VortexEditor::ProjectSettings::Create("Project settings");
  m_ProjectSettings->GetAppWindow()->SetVisibility(false);
  Cherry::AddAppWindow(m_ProjectSettings->GetAppWindow());
};

bool Editor::GetAboutAppWindowVisibility() {
  if (!m_AboutWindow)
    return false;
  return m_AboutWindow->GetAppWindow()->m_Visible;
}

void Editor::SetAboutWindowVisibility(const bool visibility) {
  if (visibility) {
    if (!m_AboutWindow) {
      m_AboutWindow = VortexEditor::AboutVortex::Create("About Vortex");

      Cherry::ApplicationSpecification spec;
      spec.SetName("About Vortex");
      spec.SetUniqueAppWindowName(m_AboutWindow->GetAppWindow()->m_Name);
      spec.MinHeight = 100;
      spec.MinWidth = 200;
      spec.Height = 450;
      spec.Width = 750;
      spec.DisableLogo = true;
      spec.DisableResize = true;
      spec.CustomTitlebar = true;
      spec.DisableWindowManagerTitleBar = true;
      spec.WindowOnlyClosable = true;
      spec.WindowSaves = false;
      spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;

      spec.UsingCloseCallback = true;
      spec.CloseCallback = [this]() {
        Cherry::DeleteAppWindow(m_AboutWindow->GetAppWindow());
        m_AboutWindow = nullptr;
      };

      spec.MenubarCallback = []() {};

      m_AboutWindow->GetAppWindow()->AttachOnNewWindow(spec);
      Cherry::AddAppWindow(m_AboutWindow->GetAppWindow());
    }

    m_AboutWindow->GetAppWindow()->SetVisibility(true);
  } else {
    if (m_AboutWindow) {
      Cherry::DeleteAppWindow(m_AboutWindow->GetAppWindow());
      m_AboutWindow = nullptr;
    }
  }
}

void Editor::SetTemplatesUtilityVisibility(const bool &visibility) {
  /*m_TemplatesUtilityAppWindow->GetAppWindow()->SetVisibility(visibility);*/
}

bool Editor::GetTemplatesUtilityVisibility() {
  /*return m_TemplatesUtilityAppWindow->GetAppWindow()->m_Visible;*/
  return false;
}

void Editor::SetProjectSettingsVisibility(const bool &visibility) {
  m_ProjectSettings->GetAppWindow()->SetVisibility(visibility);
}

bool Editor::GetProjectSettingsVisibility() {
  return m_ProjectSettings->GetAppWindow()->m_Visible;
}

void Editor::SetModulesUtilityVisibility(const bool &visibility) {
  m_ModulesUtility->GetAppWindow()->SetVisibility(visibility);
}

bool Editor::GetModulesUtilityVisibility() {
  return m_ModulesUtility->GetAppWindow()->m_Visible;
}

void Editor::SetPluginsUtilityVisibility(const bool &visibility) {
  m_PluginsUtility->GetAppWindow()->SetVisibility(visibility);
}

bool Editor::GetPluginsUtilityVisibility() {
  return m_PluginsUtility->GetAppWindow()->m_Visible;
}

void Editor::SetWelcomeVisibility(const bool &visibility) {
  // m_WelcomeAppWindow->GetAppWindow()->SetVisibility(visibility);
}

bool Editor::GetWelcomeVisibility() {
  // return m_WelcomeAppWindow->GetAppWindow()->m_Visible;
  return false;
}

std::string Editor::SpawnContentBrowser() {
  std::string label = "Content Browser ####Content Browser-" +
                      std::to_string(c_ContentBrowserInstances.size() + 1);
  std::shared_ptr<VortexEditor::ContentBrowserAppWindow> ContentBrowser =
      VortexEditor::ContentBrowserAppWindow::Create(
          label.c_str(),
          VortexMaker::GetCurrentContext()->projectDataPath.string());

  ContentBrowser->m_CutPathsCallback = VortexMaker::Cut;
  ContentBrowser->m_CopyPathsCallback = VortexMaker::Copy;
  ContentBrowser->m_PastePathsCallback = VortexMaker::PasteAllSelections;
  ContentBrowser->m_DeletePathCallback = VortexMaker::DeletePath;
  Cherry::AddAppWindow(ContentBrowser->GetAppWindow());
  c_ContentBrowserInstances.push_back(ContentBrowser);
  return label;
}

std::string Editor::SpawnDocViewer() {
  std::string label = "Doc viewer ####Doc viewer -" +
                      std::to_string(c_DocViewerInstances.size() + 1);
  std::shared_ptr<VortexEditor::DocViewerAppWindow> DocViewer =
      VortexEditor::DocViewerAppWindow::Create(label.c_str());

  Cherry::AddAppWindow(DocViewer->GetAppWindow());
  c_DocViewerInstances.push_back(DocViewer);
  return label;
}

void Editor::SpawnContentBrowserBottom() {
  const std::string new_browser = SpawnContentBrowser();

  auto dragdropstate = std::make_shared<WindowDragDropState>();

  dragdropstate->LastDraggingPlace = Cherry::DockEmplacement::DockDown;

  dragdropstate->LastDraggingWindow = CherryApp.m_Windows[0]->GetName();
  dragdropstate->DragOwner = CherryApp.m_Windows[0]->GetName();
  dragdropstate->LastDraggingAppWindow = new_browser;
  dragdropstate->LastDraggingAppWindowHost = "?loc:loc.window_names.welcome";

  CherryApp.SetCurrentDragDropState(dragdropstate);
  Application::PushRedockEvent(CherryApp.GetCurrentDragDropState());

  CherryApp.SetCurrentDragDropState(nullptr);
}

void Editor::SpawnLogsUtility() {
  std::string label = "Logs utility ####Logs utility-" +
                      std::to_string(c_LogsUtilityInstances.size() + 1);
  std::shared_ptr<VortexEditor::LogsUtilityAppWindow> LogsUtility =
      VortexEditor::LogsUtilityAppWindow::Create(label.c_str());
  Cherry::AddAppWindow(LogsUtility->GetAppWindow());
  c_LogsUtilityInstances.push_back(LogsUtility);
}

void Editor::ToggleProjectSettings() {
  c_Editor->SetProjectSettingsVisibility(
      !c_Editor->GetProjectSettingsVisibility());
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

static bool ShowDebugNumbers = false;
static bool ShowProjectType = true;
static bool ShowAccountMenu = true;
static bool ShowProjectName = true;
static bool ShowFPS = true;

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
    if (ShowProjectName) {
      drawList->AddRectFilled(rectMin, rectMax, IM_COL32(15, 15, 15, 255));
    }

    if (GetVortexBuildType() != "dev") {
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() -
                               CherryGUI::CalcTextSize("DEV").x - 20.0f);

      if (ShowProjectType) {
        CherryGUI::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "DEV");
      }
    } else {
      if (VortexMaker::GetCurrentContext()->type == "tool") {
        CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() -
                                 CherryGUI::CalcTextSize("Tool").x - 20.0f);

        if (ShowProjectType) {
          CherryGUI::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "Tool");
        }
      } else if (VortexMaker::GetCurrentContext()->type == "project") {
        CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() -
                                 CherryGUI::CalcTextSize("Project").x - 20.0f);

        if (ShowProjectType) {
          CherryGUI::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "Project");
        }
      }
    }

    CherryGUI::SetCursorScreenPos(circlePos);
    CherryGUI::InvisibleButton("circleButton",
                               ImVec2(circleRadius * 2, circleRadius * 2));
    if (ShowAccountMenu) {

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
    }

    ImVec2 textPos =
        ImVec2(rectMin.x + rectanglePaddingX,
               rectMin.y + (rectMax.y - rectMin.y - textSize.y) * 0.5f +
                   rectanglePaddingY - 10);

    CherryGUI::SetCursorScreenPos(textPos);

    if (CherryGUI::IsItemClicked(ImGuiMouseButton_Left)) {
      CherryGUI::OpenPopup("projectMenu");
    }

    if (ShowProjectName) {
      CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), text);
    }

    CherryGUI::GetFont()->Scale = oldSize;
    CherryGUI::PopFont();

    CherryGUI::PopFont();
  });
  Cherry::Application *app = new Cherry::Application(spec);
  app->SetFavIconPath(Cherry::Application::CookPath("resources/imgs/icon.png"));
  app->AddFont("Consola",
               Cherry::Application::CookPath("resources/fonts/consola.ttf"),
               50.0f);
  app->AddFont("Clash",
               Cherry::Application::CookPath("resources/fonts/clash.ttf"),
               70.0f);

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
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 8.0f));

    static bool t;

    if (CherryGUI::BeginMenu("Project")) {

      if (CherryGUI::MenuItem("Welcome", "",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_doc.png")),
                              false)) {
        c_Editor->SetWelcomeVisibility(!c_Editor->GetWelcomeVisibility());
      }

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
                              c_Editor->GetPluginsUtilityVisibility())) {
        c_Editor->SetPluginsUtilityVisibility(
            !c_Editor->GetPluginsUtilityVisibility());
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
        c_Editor->SetProjectSettingsVisibility(
            !c_Editor->GetProjectSettingsVisibility());
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
      if (CherryGUI::MenuItem("Undo", "Change selected language",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_doc.png")),
                              false)) {
        // undo main callback
      }
      if (CherryGUI::MenuItem("Redo", "Change selected theme",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_doc.png")),
                              false)) {
        // redo main callback
      }
      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu("Window")) {
      CherryKit::SeparatorText("Upper bar");

      if (CherryGUI::MenuItem("Show project name", "",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_info.png")),
                              ShowProjectName)) {
        ShowProjectName = !ShowProjectName;
      }
      if (CherryGUI::MenuItem("Show project nature", "",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_info.png")),
                              ShowProjectType)) {
        ShowProjectType = !ShowProjectType;
      }
      if (CherryGUI::MenuItem("Show my profile ", "",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_info.png")),
                              ShowAccountMenu)) {
        ShowAccountMenu = !ShowAccountMenu;
      }
      if (CherryGUI::MenuItem("Show errors and warnings ", "",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_info.png")),
                              ShowDebugNumbers)) {
        ShowDebugNumbers = !ShowDebugNumbers;
      }
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
      CherryKit::SeparatorText("Quick settings");

      if (CherryGUI::MenuItem("Switch language", "Change selected language",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_doc.png")),
                              false)) {
        //
      }
      if (CherryGUI::MenuItem("Switch theme", "Change selected theme",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_doc.png")),
                              false)) {
        //
      }

      CherryGUI::EndMenu();
    }

    if (CherryGUI::BeginMenu("Help")) {
      if (CherryGUI::MenuItem("Welcome", "",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_doc.png")),
                              false)) {
        c_Editor->SetWelcomeVisibility(!c_Editor->GetWelcomeVisibility());
      }
      CherryKit::SeparatorText("Learning, documentation");

      if (CherryGUI::MenuItem("Documentation", "Open embedded documentations",
                              Cherry::GetTexture(Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_doc.png")),
                              false)) {
        c_Editor->SpawnDocViewer();
      }

      CherryKit::SeparatorText("About");

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

    CherryGUI::PopStyleVar(2);
    CherryGUI::PopStyleColor(2);
  }

  );

  c_Editor = std::make_shared<Editor>();
  return app;
}

int VortexMaker::VortexEditor(int argc, char **argv) {
  return Cherry::ThirdMain(argc, argv, CreateEditor);
}