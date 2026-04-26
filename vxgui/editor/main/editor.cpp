#include "./editor.hpp"

std::string GetVortexBuildType() {
  return VORTEX_BUILD;
}

Editor::Editor() {
  welcome_window_ = vxe::Welcome::create("?loc:loc.window_names.welcome");
  welcome_window_->get_app_window()->SetVisibility(true);
  Cherry::AddAppWindow(welcome_window_->get_app_window());

  modules_utility_window_ = vxe::ModulesUtility::create("Modules utily");
  modules_utility_window_->get_app_window()->SetVisibility(false);
  Cherry::AddAppWindow(modules_utility_window_->get_app_window());

  plugins_utility_window_ = vxe::PluginsUtility::create("Plugins utily");
  plugins_utility_window_->get_app_window()->SetVisibility(false);
  Cherry::AddAppWindow(plugins_utility_window_->get_app_window());

  project_settings_window_ = vxe::ProjectSettings::create("Project settings");
  project_settings_window_->get_app_window()->SetVisibility(false);
  Cherry::AddAppWindow(project_settings_window_->get_app_window());
};

bool Editor::get_credits_visibility() {
  if (!credits_window_)
    return false;
  return credits_window_->get_app_window()->m_Visible;
}

void Editor::set_credits_visibility(const bool visibility) {
  if (visibility) {
    if (!credits_window_) {
      credits_window_ = vxe::Credits::create("Credits");

      Cherry::ApplicationSpecification spec;
      spec.SetName("Credits");
      spec.SetUniqueAppWindowName(credits_window_->get_app_window()->m_Name);
      spec.MinHeight = 200;
      spec.MinWidth = 100;
      spec.Height = 750;
      spec.Width = 450;
      spec.DisableLogo = true;
      spec.DisableResize = true;
      spec.CustomTitlebar = true;
      spec.DisableWindowManagerTitleBar = true;
      spec.WindowOnlyClosable = true;
      spec.WindowSaves = false;
      spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;

      spec.UsingCloseCallback = true;
      spec.CloseCallback = [this]() {
        Cherry::DeleteAppWindow(credits_window_->get_app_window());
        credits_window_ = nullptr;
      };

      spec.MenubarCallback = []() { };

      credits_window_->get_app_window()->AttachOnNewWindow(spec);
      Cherry::AddAppWindow(credits_window_->get_app_window());
    }

    credits_window_->get_app_window()->SetVisibility(true);
  } else {
    if (credits_window_) {
      Cherry::DeleteAppWindow(credits_window_->get_app_window());
      credits_window_ = nullptr;
    }
  }
}

bool Editor::get_about_visibility() {
  if (!about_window_)
    return false;
  return about_window_->get_app_window()->m_Visible;
}

void Editor::set_about_visibility(const bool visibility) {
  if (visibility) {
    if (!about_window_) {
      about_window_ = vxe::AboutVortex::create("About Vortex");

      Cherry::ApplicationSpecification spec;
      spec.SetName("About Vortex");
      spec.SetUniqueAppWindowName(about_window_->get_app_window()->m_Name);
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
        Cherry::DeleteAppWindow(about_window_->get_app_window());
        about_window_ = nullptr;
      };

      spec.MenubarCallback = []() { };

      about_window_->get_app_window()->AttachOnNewWindow(spec);
      Cherry::AddAppWindow(about_window_->get_app_window());
    }

    about_window_->get_app_window()->SetVisibility(true);
  } else {
    if (about_window_) {
      Cherry::DeleteAppWindow(about_window_->get_app_window());
      about_window_ = nullptr;
    }
  }
}

bool Editor::get_about_project_visibility() {
  if (!about_project_window_)
    return false;
  return about_project_window_->get_app_window()->m_Visible;
}

void Editor::set_about_project_visibility(const bool visibility) {
  if (visibility) {
    if (!about_project_window_) {
      about_project_window_ = vxe::AboutProject::create("About this project");

      Cherry::ApplicationSpecification spec;
      spec.SetName("About this project");
      spec.SetUniqueAppWindowName(about_project_window_->get_app_window()->m_Name);
      spec.MinHeight = 100;
      spec.MinWidth = 200;
      spec.Height = 380;
      spec.Width = 520;
      spec.DisableLogo = true;
      spec.DisableResize = true;
      spec.CustomTitlebar = true;
      spec.DisableWindowManagerTitleBar = true;
      spec.WindowOnlyClosable = true;
      spec.WindowSaves = false;
      spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;

      spec.UsingCloseCallback = true;
      spec.CloseCallback = [this]() {
        Cherry::DeleteAppWindow(about_project_window_->get_app_window());
        about_project_window_ = nullptr;
      };

      spec.MenubarCallback = []() { };

      about_project_window_->get_app_window()->AttachOnNewWindow(spec);
      Cherry::AddAppWindow(about_project_window_->get_app_window());
    }

    about_project_window_->get_app_window()->SetVisibility(true);
  } else {
    if (about_project_window_) {
      Cherry::DeleteAppWindow(about_project_window_->get_app_window());
      about_project_window_ = nullptr;
    }
  }
}
void Editor::set_templates_utility_visibility(const bool &visibility) {
  /*m_TemplatesUtilityAppWindow->GetAppWindow()->SetVisibility(visibility);*/
}

bool Editor::get_templates_utility_visibility() {
  /*return m_TemplatesUtilityAppWindow->GetAppWindow()->m_Visible;*/
  return false;
}

void Editor::set_project_settings_visibility(const bool &visibility, const std::string &tab = "") {
  if (!tab.empty()) {
    project_settings_window_->load_tab_user_want(tab);
  }
  project_settings_window_->get_app_window()->SetVisibility(visibility);
  if (visibility) {
    CherryGUI::SetWindowFocus(project_settings_window_->get_app_window()->m_IdName.c_str());
  }
}

bool Editor::get_project_settings_visibility() {
  return project_settings_window_->get_app_window()->m_Visible;
}

void Editor::set_modules_utility_visibility(const bool &visibility) {
  modules_utility_window_->get_app_window()->SetVisibility(visibility);
}

bool Editor::get_modules_utility_visibility() {
  return modules_utility_window_->get_app_window()->m_Visible;
}

void Editor::set_plugins_utility_visibility(const bool &visibility) {
  plugins_utility_window_->get_app_window()->SetVisibility(visibility);
}

bool Editor::get_plugins_utility_visibility() {
  return plugins_utility_window_->get_app_window()->m_Visible;
}

void Editor::set_welcome_visibility(const bool &visibility) {
  welcome_window_->get_app_window()->SetVisibility(visibility);
}

bool Editor::get_welcome_visibility() {
  return welcome_window_->get_app_window()->m_Visible;
}

std::string Editor::spawn_content_browser() {
  std::string label = "Content Browser ####Content Browser-" + std::to_string(content_browser_instances_.size() + 1);
  std::shared_ptr<vxe::ContentBrowser> ContentBrowser =
      vxe::ContentBrowser::create(label.c_str(), vxe::get_current_context()->projectDataPath.string());

  ContentBrowser->set_cut_paths_callback(vxe::Cut);
  ContentBrowser->set_copy_paths_callback(vxe::Copy);
  ContentBrowser->set_paste_paths_callback(vxe::PasteAllSelections);
  ContentBrowser->set_delete_path_callback(vxe::DeletePath);
  Cherry::AddAppWindow(ContentBrowser->get_app_window());
  content_browser_instances_.push_back(ContentBrowser);
  return label;
}

std::string Editor::SpawnDocViewer() {
  std::string label = "Doc viewer ####Doc viewer -" + std::to_string(doc_viewer_instances_.size() + 1);
  std::shared_ptr<vxe::DocViewer> DocViewer = vxe::DocViewer::create(label.c_str());

  Cherry::AddAppWindow(DocViewer->get_app_window());
  doc_viewer_instances_.push_back(DocViewer);
  return label;
}

void Editor::spawn_content_browser_on_bottom() {
  const std::string new_browser = spawn_content_browser();

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

void Editor::spawn_logs_utility() {
  std::string label = "Logs utility ####Logs utility-" + std::to_string(logs_utility_instances_.size() + 1);
  std::shared_ptr<vxe::LogsUtility> LogsUtility = vxe::LogsUtility::create(label.c_str());
  Cherry::AddAppWindow(LogsUtility->get_app_window());
  logs_utility_instances_.push_back(LogsUtility);
}

void Editor::toggle_project_settings() {
  c_Editor->set_project_settings_visibility(!c_Editor->get_project_settings_visibility());
}

// Frame task
void RebuildCherryTheme() {
  if (vxe::IsThemeNeedsRebuild()) {
    for (auto t : vxe::get_current_context()->IO.themes) {
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
    auto selected_theme = vxe::GetSelectedTheme();
    if (selected_theme) {
      CherryApp.SetTheme(selected_theme->label);
    }

    // set override themes
    vxe::ThemeRebuilded();
  }
}

static bool ShowDebugNumbers = false;
static bool ShowProjectType = true;
static bool ShowAccountMenu = false;
static bool ShowProjectName = true;
static bool ShowFPS = true;

void Editor::render_framebar() {
  float oldSize = CherryGUI::GetFont()->Scale;
  CherryGUI::PushFont(CherryGUI::GetFont());

  const char *text = vxe::get_current_context()->name.c_str();
  CherryGUI::GetFont()->Scale *= 0.84;
  CherryGUI::PushFont(CherryGUI::GetFont());

  ImVec2 textSize = CherryGUI::CalcTextSize(text);

  float circleRadius = 10.0f;
  float circlePadding = 12.0f;
  float rectanglePaddingX = 15.0f;
  float rectanglePaddingY = 25.0f;

  ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

  float yOffset = 5.0f;

  ImVec2 circlePos = ImVec2(cursorPos.x - 12 + circleRadius, cursorPos.y + textSize.y * 0.1f + 6);

  ImVec2 rectMin = ImVec2(cursorPos.x + circleRadius + circlePadding, cursorPos.y - 38 + yOffset);
  ImVec2 rectMax =
      ImVec2(rectMin.x + textSize.x + 2 * rectanglePaddingX, cursorPos.y + textSize.y + 2 * rectanglePaddingY - 45);

  ImDrawList *drawList = CherryGUI::GetWindowDrawList();
  if (ShowProjectName) {
    drawList->AddRectFilled(rectMin, rectMax, IM_COL32(15, 15, 15, 255));
  }

  if (GetVortexBuildType() == "dev") {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() - CherryGUI::CalcTextSize("DEV").x - 20.0f);

    if (ShowProjectType) {
      CherryGUI::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "DEV");
    }
  } else {
    if (vxe::get_current_context()->type == "tool") {
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() - CherryGUI::CalcTextSize("Tool").x - 20.0f);

      if (ShowProjectType) {
        CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/tools.png"), 13, 13);
        CherryGUI::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "Tool");
      }
    } else if (vxe::get_current_context()->type == "project") {
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() - CherryGUI::CalcTextSize("Project").x - 20.0f);

      if (ShowProjectType) {
        CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/light.png"), 13, 13);
        CherryGUI::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "Project");
      }
    }
  }

  if (ShowDebugNumbers) {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() - 150.0f);
    CherryGUI::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "1 Bug 1 Warn");
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 150.0f);
  }

  CherryGUI::SetCursorScreenPos(circlePos);
  CherryGUI::InvisibleButton("circleButton", ImVec2(circleRadius * 2, circleRadius * 2));
  if (ShowAccountMenu) {
    if (CherryGUI::IsItemHovered()) {
      drawList->AddCircle(circlePos, circleRadius + 1.0f, IM_COL32(200, 200, 200, 255), 32, 2.0f);
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
  }

  ImVec2 textPos = ImVec2(
      rectMin.x + rectanglePaddingX, rectMin.y + (rectMax.y - rectMin.y - textSize.y) * 0.5f + rectanglePaddingY - 10);

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
}

void Editor::render_menubar() {
  if (vxe::IsThemeNeedsRebuild()) {
    CherryApp.m_Themes.clear();
    for (auto t : vxe::get_current_context()->IO.themes) {
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
    auto selected_theme = vxe::GetSelectedTheme();
    if (selected_theme) {
      CherryApp.SetTheme(selected_theme->label);
    }

    // set override themes
    vxe::ThemeRebuilded();
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
  auto ctx = vxe::get_current_context();

  if (CherryGUI::BeginMenu("Project")) {
    if (CherryGUI::MenuItem(
            "Welcome",
            "",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_hand.png")),
            get_welcome_visibility())) {
      set_welcome_visibility(!get_welcome_visibility());
    }

    CherryKit::SeparatorText("Logical contents");

    if (CherryGUI::MenuItem(
            "Modules utility",
            "Open the modules utility",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_bricksearch.png")),
            get_modules_utility_visibility())) {
      set_modules_utility_visibility(!get_modules_utility_visibility());
    }
    if (CherryGUI::MenuItem(
            "Plugins utility",
            "Open the plugins utility",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_plugin.png")),
            get_plugins_utility_visibility())) {
      set_plugins_utility_visibility(!get_plugins_utility_visibility());
    }

    CherryKit::SeparatorText("Static contents");
    if (CherryGUI::MenuItem(
            "Templates utility",
            "Open the templates utility",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
            get_templates_utility_visibility())) {
      set_templates_utility_visibility(!get_templates_utility_visibility());
    }
    if (CherryGUI::MenuItem(
            "Contents utility",
            "Open the contents utility",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
            get_templates_utility_visibility())) {
    }

    CherryKit::SeparatorText("Configurations & Settings");
    if (CherryGUI::MenuItem(
            "Project settings",
            "Open the main settings of this project",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png")),
            get_project_settings_visibility())) {
      set_project_settings_visibility(!get_project_settings_visibility());
    }

    CherryKit::SeparatorText("Other");

    if (CherryGUI::MenuItem(
            "About Vortex",
            "About the Vortex Editor",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
            get_about_visibility())) {
      set_about_visibility(!get_about_visibility());
    }

    if (CherryGUI::MenuItem(
            "About this project",
            "About the current project",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
            get_about_project_visibility())) {
      set_about_project_visibility(!get_about_project_visibility());
    }
    if (CherryGUI::MenuItem(
            "Credits",
            "Vortex team, developers and contributors",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_book.png")),
            get_credits_visibility())) {
      set_credits_visibility(!get_credits_visibility());
    }

    CherryGUI::EndMenu();
  }

  if (CherryGUI::BeginMenu("Edit")) {
    // TODO : General edit functions of vortex

    const auto &items = ctx->editMenuItems;

    if (items.empty()) {
      CherryNextComponent.SetProperty("text_color", "#343434");
      CherryKit::TextSimple("Nothing to edit");
    }

    std::vector<std::string> sectionOrder;
    std::unordered_map<std::string, std::vector<const EditMenuItem *>> grouped;

    for (const auto &item : items) {
      const std::string &sec = item.section.empty() ? "" : item.section;
      if (grouped.find(sec) == grouped.end())
        sectionOrder.push_back(sec);
      grouped[sec].push_back(&item);
    }

    bool firstSection = true;
    for (const auto &sec : sectionOrder) {
      if (!sec.empty())
        CherryKit::SeparatorText(sec.c_str());
      else if (!firstSection)
        CherryGUI::Separator();

      firstSection = false;

      for (const auto *item : grouped[sec]) {
        ImTextureID tex = item->logo.empty() ? nullptr : Cherry::GetTexture(Cherry::GetPath(item->logo));

        if (CherryGUI::MenuItem(item->title.c_str(), "", tex, false)) {
          if (item->action)
            item->action();
        }
      }
    }

    CherryGUI::EndMenu();
  }

  if (CherryGUI::BeginMenu("Window")) {
    CherryKit::SeparatorText("Upper bar");

    if (CherryGUI::MenuItem(
            "Show project name",
            "",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
            ShowProjectName)) {
      ShowProjectName = !ShowProjectName;
    }
    if (CherryGUI::MenuItem(
            "Show project nature",
            "",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
            ShowProjectType)) {
      ShowProjectType = !ShowProjectType;
    }
    if (CherryGUI::MenuItem(
            "Show my profile ",
            "",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
            ShowAccountMenu)) {
      ShowAccountMenu = !ShowAccountMenu;
    }
    if (CherryGUI::MenuItem(
            "Show errors and warnings ",
            "",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
            ShowDebugNumbers)) {
      ShowDebugNumbers = !ShowDebugNumbers;
    }
    CherryGUI::EndMenu();
  }

  if (CherryGUI::BeginMenu("Tools")) {
    CherryKit::SeparatorText("Contents, managment");

    if (CherryGUI::MenuItem(
            "Content Browser",
            "Open a content browser",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_files.png")),
            false)) {
      spawn_content_browser();
    }

    CherryKit::SeparatorText("Console, logs, debugging");
    if (CherryGUI::MenuItem(
            "Console logs",
            "Open a console with logs",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/frame_cli.png")),
            false)) {
      spawn_logs_utility();
    }

    CherryGUI::EndMenu();
  }

  if (CherryGUI::BeginMenu("Options")) {
    CherryKit::SeparatorText("Quick settings");

    if (CherryGUI::MenuItem(
            "Switch language",
            "Change selected language",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_language.png")),
            false)) {
      set_project_settings_visibility(true, "language");
    }
    if (CherryGUI::MenuItem(
            "Switch theme",
            "Change selected theme",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_palette.png")),
            false)) {
      set_project_settings_visibility(true, "theme");
    }

    CherryGUI::EndMenu();
  }

  if (CherryGUI::BeginMenu("Help")) {
    if (CherryGUI::MenuItem(
            "Welcome", "", Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_hand.png")), false)) {
      set_welcome_visibility(!get_welcome_visibility());
    }
    CherryKit::SeparatorText("Learning, documentation");

    if (CherryGUI::MenuItem(
            "Documentation",
            "Open embedded documentations",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_journal.png")),
            false)) {
      SpawnDocViewer();
    }

    CherryKit::SeparatorText("About");

    if (CherryGUI::MenuItem(
            "About Vortex",
            "About the Vortex Editor",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
            get_about_visibility())) {
      set_about_visibility(!get_about_visibility());
    }

    if (CherryGUI::MenuItem(
            "About this project",
            "About the current project",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")),
            get_about_project_visibility())) {
      set_about_project_visibility(!get_about_project_visibility());
    }
    if (CherryGUI::MenuItem(
            "Credits",
            "Vortex team, developers and contributors",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_book.png")),
            get_credits_visibility())) {
      set_credits_visibility(!get_credits_visibility());
    }

    CherryGUI::EndMenu();
  }

  if (ctx->customMenus.size() > 0) {
    {
      ImVec2 pos = CherryGUI::GetCursorScreenPos();
      float height = CherryGUI::GetFrameHeight();
      float thickness = 1.5f;

      CherryGUI::GetWindowDrawList()->AddRectFilled(
          ImVec2(pos.x, pos.y), ImVec2(pos.x + thickness, pos.y + height), IM_COL32(89, 89, 89, 155));
      CherryGUI::Dummy(ImVec2(thickness + 4.0f, 0));
    }
    for (auto &menu : ctx->customMenus) {
      if (CherryGUI::BeginMenu(menu.title.c_str())) {
        if (menu.render) {
          menu.render();
        }
        CherryGUI::EndMenu();
      }
    }
  }

  CherryGUI::PopStyleVar(2);
  CherryGUI::PopStyleColor(2);

  // Clear edits menuitems from previous render.
  vxe::ClearEditMenuItem();

  // Clear custom menus from previous render.
  vxe::ClearCustomMenus();
}

Cherry::Application *CreateEditor(int argc, char **argv) {
  c_Editor = std::make_shared<Editor>();
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
  spec.SetFramebarCallback([]() { c_Editor->render_framebar(); });
  Cherry::Application *app = new Cherry::Application(spec);
  app->SetFavIconPath(Cherry::Application::CookPath("resources/imgs/icon.png"));
  app->AddFont("Consola", Cherry::Application::CookPath("resources/fonts/consola.ttf"), 50.0f);
  app->AddFont("Clash", Cherry::Application::CookPath("resources/fonts/clash.ttf"), 70.0f);

  app->AddFont("JetBrainsMono", Cherry::Application::CookPath("resources/fonts/JetBrainsMono-Regular.ttf"), 40.0f);

  app->AddLocale("fr", Cherry::GetPath("resources/locales/fr.json"));
  app->AddLocale("en", Cherry::GetPath("resources/locales/en.json"));
  app->SetDefaultLocale("en");
  app->SetLocale("en");

  // Cherry INIT from mopdules
  for (auto mod : vxe::get_current_context()->IO.em) {
    if (mod) {
      mod->init_ui();
    }
  }

  for (auto &modules : vxe::get_current_context()->IO.em) {
    modules->RefreshMainWindow();
  }

  app->SetMenubarCallback([]() { c_Editor->render_menubar(); }

  );

  return app;
}

int vxe::VortexEditor(int argc, char **argv) {
  return Cherry::ThirdMain(argc, argv, CreateEditor);
}