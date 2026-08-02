//
//  add_window.cpp
//  Sources for the "add window" of the content browser
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./add_window.hpp"

namespace vxe {

  namespace {

    struct TileEntry {
      std::string id;
      ImTextureID icon;
      std::string title;
      bool enabled = true;
      std::function<void()> on_click;
    };

    bool MatchesSearch(const std::string &title, const std::string &search) {
      if (search.empty()) {
        return true;
      }
      std::string lowerTitle = title;
      std::string lowerSearch = search;
      std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
      std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
      return lowerTitle.find(lowerSearch) != std::string::npos;
    }

    bool DrawTile(const char *id, ImTextureID icon, const std::string &title, float w, float h, bool enabled) {
      ImVec2 pos = CherryGUI::GetCursorScreenPos();
      ImDrawList *dl = CherryGUI::GetWindowDrawList();

      bool clicked = false;
      bool hovered = false;

      if (enabled) {
        clicked = CherryGUI::InvisibleButton(id, ImVec2(w, h));
        hovered = CherryGUI::IsItemHovered();
        if (hovered) {
          CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
      } else {
        CherryGUI::Dummy(ImVec2(w, h));
      }

      ImU32 bg = enabled ? (hovered ? IM_COL32(58, 58, 58, 255) : IM_COL32(40, 40, 40, 255)) : IM_COL32(30, 30, 30, 255);
      ImU32 border =
          enabled ? (hovered ? IM_COL32(90, 130, 220, 255) : IM_COL32(65, 65, 65, 255)) : IM_COL32(50, 50, 50, 255);
      ImU32 textColor = enabled ? IM_COL32(215, 215, 215, 255) : IM_COL32(110, 110, 110, 255);

      dl->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h), bg, 5.0f);
      dl->AddRect(pos, ImVec2(pos.x + w, pos.y + h), border, 5.0f, 0, 1.2f);

      float iconSize = 28.0f;
      ImVec2 iconPos(pos.x + (w - iconSize) * 0.5f, pos.y + 12.0f);
      if (icon) {
        ImVec4 tint = enabled ? ImVec4(1, 1, 1, 1) : ImVec4(1, 1, 1, 0.35f);
        dl->AddImage(
            icon,
            iconPos,
            ImVec2(iconPos.x + iconSize, iconPos.y + iconSize),
            ImVec2(0, 0),
            ImVec2(1, 1),
            CherryGUI::ColorConvertFloat4ToU32(tint));
      }

      ImVec2 textSize = CherryGUI::CalcTextSize(title.c_str());
      float textX = pos.x + (w - textSize.x) * 0.5f;
      if (textSize.x > w - 8.0f) {
        textX = pos.x + 4.0f;
      }
      ImVec2 textPos(textX, iconPos.y + iconSize + 8.0f);
      dl->AddText(textPos, textColor, title.c_str());

      return clicked;
    }

    void
    DrawTileGrid(const std::vector<TileEntry> &entries, float available_width, float min_tile_w, float tile_h, float gap) {
      if (entries.empty()) {
        return;
      }

      int columns = (int)((available_width + gap) / (min_tile_w + gap));
      if (columns < 1) {
        columns = 1;
      }
      if (columns > (int)entries.size()) {
        columns = (int)entries.size();
      }

      float tile_w = (available_width - (columns - 1) * gap) / columns;

      for (size_t i = 0; i < entries.size(); ++i) {
        const auto &e = entries[i];

        if (DrawTile(e.id.c_str(), e.icon, e.title, tile_w, tile_h, e.enabled)) {
          if (e.on_click) {
            e.on_click();
          }
        }

        bool lastInRow = ((i + 1) % columns == 0);
        bool lastEntry = (i + 1 == entries.size());

        if (!lastInRow && !lastEntry) {
          CherryGUI::SameLine(0.0f, gap);
        } else if (!lastEntry) {
          CherryGUI::Dummy(ImVec2(0.0f, gap));
        }
      }

      CherryGUI::Dummy(ImVec2(0.0f, gap));
    }

    bool DrawRow(const char *id, ImTextureID icon, const std::string &title, const std::string &description, float width) {
      float height = 56.0f;
      float padding = 10.0f;
      float iconSize = 34.0f;

      ImVec2 pos = CherryGUI::GetCursorScreenPos();
      ImDrawList *dl = CherryGUI::GetWindowDrawList();

      bool clicked = CherryGUI::InvisibleButton(id, ImVec2(width, height));
      bool hovered = CherryGUI::IsItemHovered();

      if (hovered) {
        CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
      }

      ImU32 bg = hovered ? IM_COL32(52, 52, 52, 255) : IM_COL32(38, 38, 38, 255);
      dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), bg, 5.0f);

      ImVec2 iconPos(pos.x + padding, pos.y + (height - iconSize) * 0.5f);
      if (icon) {
        dl->AddImage(icon, iconPos, ImVec2(iconPos.x + iconSize, iconPos.y + iconSize));
      }

      float textX = iconPos.x + iconSize + padding;
      dl->AddText(ImVec2(textX, pos.y + 9.0f), IM_COL32(230, 230, 230, 255), title.c_str());
      dl->AddText(ImVec2(textX, pos.y + 30.0f), IM_COL32(150, 150, 150, 255), description.c_str());

      return clicked;
    }
    // TODO Remove tht and use vxe::
    std::string detect_current_platform() {
#if defined(_WIN32)
      return "windows";
#elif defined(__APPLE__)
      return "macos";
#elif defined(__linux__)
      return "linux";
#else
      return "unknown";
#endif
    }

    std::string detect_current_arch() {
#if defined(__aarch64__) || defined(_M_ARM64)
      return "arm64";
#elif defined(__x86_64__) || defined(_M_X64)
      return "x86_64";
#else
      return "unknown";
#endif
    }

    std::vector<int> parse_version_numbers(const std::string &v) {
      std::vector<int> parts;
      std::stringstream ss(v);
      std::string item;
      while (std::getline(ss, item, '.')) {
        try {
          parts.push_back(std::stoi(item));
        } catch (...) {
          parts.push_back(0);
        }
      }
      return parts;
    }

    bool version_greater(const std::string &a, const std::string &b) {
      auto pa = parse_version_numbers(a);
      auto pb = parse_version_numbers(b);
      size_t n = (std::max)(pa.size(), pb.size());
      for (size_t i = 0; i < n; i++) {
        int va = i < pa.size() ? pa[i] : 0;
        int vb = i < pb.size() ? pb[i] : 0;
        if (va != vb) {
          return va > vb;
        }
      }
      return false;
    }

    std::string extract_major(const std::string &version, int keep = 2) {
      auto parts = parse_version_numbers(version);
      std::string result;
      for (int i = 0; i < keep && i < (int)parts.size(); i++) {
        if (i > 0) {
          result += ".";
        }
        result += std::to_string(parts[i]);
      }
      return result;
    }

    std::string native_platform_label(const std::string &platform, const std::string &arch) {
      std::string pretty = platform;
      if (platform == "windows")
        pretty = "Windows";
      else if (platform == "macos")
        pretty = "macOS";
      else if (platform == "linux")
        pretty = "Linux";
      return pretty + " (" + arch + ")";
    }

  }  // namespace

  ContentFetchResult fetch_content_from_flashlink(const std::string &flashlink) {
    ContentFetchResult result;

    if (vxe::get_current_context()->disconnected) {
      result.error = "No internet connexion";
      return result;
    }

    const std::string prefix = "con:";
    if (flashlink.rfind(prefix, 0) != 0) {
      result.error = "Invalid flashlink";
      return result;
    }
    std::string uuid = flashlink.substr(prefix.size());

    auto &ctx = *vxe::get_current_context();

    std::string content_url = "http://api.infinite.si:9000/api/garagevortex/get_content?uuid=" + uuid;
    std::string content_body;
    try {
      content_body = ctx.net.GET(content_url);
    } catch (...) {
      result.error = "Unable to fetch the service API";
      return result;
    }

    if (content_body.empty()) {
      result.error = "Empty response from the API";
      return result;
    }

    nlohmann::json content_json;
    try {
      content_json = nlohmann::json::parse(content_body);
    } catch (...) {
      result.error = "Invalid JSON";
      return result;
    }

    ContentInfo info;
    info.uuid = content_json.value("uuid", "");
    info.name = content_json.value("name", "");
    info.proper_name = content_json.value("proper_name", "");
    info.description = content_json.value("description", "");
    info.picture_link = content_json.value("picture_link", "");
    info.banner_link = content_json.value("banner_link", "");
    info.host = content_json.value("host", "");
    info.host_link = content_json.value("host_link", "");
    info.certified = content_json.value("certified", false);
    info.state = content_json.value("state", "");

    result.info = info;

    std::string releases_url =
        "https://api.infinite.si/api/garagevortex/get_content_releases_summary?parent_uuid=" + uuid + "&kind=content";

    std::string releases_body;
    try {
      releases_body = ctx.net.GET(releases_url);
    } catch (...) {
      result.success = true;
      return result;
    }

    if (releases_body.empty()) {
      result.success = true;
      return result;
    }

    nlohmann::json releases_json;
    try {
      releases_json = nlohmann::json::parse(releases_body);
    } catch (...) {
      result.success = true;
      return result;
    }

    if (!releases_json.contains("releases") || !releases_json["releases"].is_array()) {
      result.success = true;
      return result;
    }

    std::string current_platform = detect_current_platform();
    std::string current_arch = detect_current_arch();
    std::string current_major = extract_major(VORTEX_VERSION);

    std::vector<ContentRelease> parsed;

    for (auto &item : releases_json["releases"]) {
      if (!item.is_object()) {
        continue;
      }

      ContentRelease rel;
      rel.uuid = item.value("uuid", "");
      rel.name = item.value("name", "");

      std::vector<std::string> tokens;
      std::stringstream ss(rel.name);
      std::string tok;
      while (std::getline(ss, tok, ':')) {
        tokens.push_back(tok);
      }

      if (tokens.size() != 5 || tokens[0] != "vx") {
        continue;
      }

      rel.platform = tokens[1];
      rel.arch = tokens[2];
      rel.major = tokens[3];
      rel.version = tokens[4];

      parsed.push_back(rel);
    }

    std::vector<ContentRelease> cross;
    for (auto &rel : parsed) {
      if (rel.platform == "cross") {
        cross.push_back(rel);
      }
    }
    std::sort(cross.begin(), cross.end(), [](const ContentRelease &a, const ContentRelease &b) {
      return version_greater(a.version, b.version);
    });

    std::vector<ContentRelease> compatible;
    for (auto &rel : parsed) {
      if (rel.platform == current_platform && rel.arch == current_arch && rel.major == current_major) {
        compatible.push_back(rel);
      }
    }

    if (compatible.empty()) {
      for (auto &rel : parsed) {
        if (rel.platform == current_platform && rel.arch == current_arch) {
          compatible.push_back(rel);
        }
      }
    }

    std::sort(compatible.begin(), compatible.end(), [](const ContentRelease &a, const ContentRelease &b) {
      return version_greater(a.version, b.version);
    });

    result.compatible_releases = compatible;
    result.compatible_cross_releases = cross;
    result.success = true;
    return result;
  }

  AddWindow::AddWindow(const std::string &name, const std::string &path) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [this]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(8.0f);
    app_window_->SetInternalPaddingY(8.0f);

    creation_path_ = path;

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::shared_ptr<Cherry::AppWindow> &AddWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<AddWindow> AddWindow::create(const std::string &name, const std::string &path) {
    auto instance = std::shared_ptr<AddWindow>(new AddWindow(name, path));
    instance->setup_render_callback();
    return instance;
  }

  void AddWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void AddWindow::startSearch(const std::string &flashlink) {
    uint64_t my_token = ++search_token;

    manual_search_error_.clear();
    current_flashlink = flashlink;
    selected_release_index_ = 0;
    platform_mode_initialized_ = false;
    install_progress_.reset();
    state = FlashLinkState::Loading;

    auto self = shared_from_this();
    std::thread([self, my_token, flashlink]() {
      ContentFetchResult res = vxe::fetch_content_from_flashlink(flashlink);

      if (!self || self->search_token.load() != my_token) {
        return;
      }

      {
        std::lock_guard<std::mutex> lock(self->result_mutex_);
        self->fetch_result_ = res;
      }

      self->state = res.success ? FlashLinkState::Ready : FlashLinkState::Error;
    }).detach();
  }

  bool AddWindow::tryProcessCandidate(const std::string &raw, bool clear_input_on_success) {
    if (raw.empty() || raw.length() >= 70) {
      return false;
    }

    bool all_valid = true;
    for (unsigned char c : raw) {
      if (!is_base64(c) && c != '=') {
        all_valid = false;
        break;
      }
    }

    if (!all_valid) {
      return false;
    }

    std::string decoded = base64_decode(raw);

    if (!is_content_flashlink(decoded)) {
      return false;
    }

    detected = true;
    decoded_text = decoded;

    startSearch(decoded);

    if (clear_input_on_success) {
      input_buffer[0] = '\0';
    }

    return true;
  }

  void AddWindow::render() {
    if (show_content_flash_link_) {
      render_content_flash_link();
      return;
    }

    render_default();
  }

  void AddWindow::render_content_flash_link() {
    CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    CherryGUI::BeginChild("##content_flash_link", ImVec2(0.0f, 0.0f), true);
    CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText("Back", Cherry::GetPath("resources/imgs/icons/misc/icon_back.png"))
            .GetDataAs<bool>("isClicked")) {
      show_content_flash_link_ = false;
    }

    CherryGUI::Spacing();

    CherryGUI::TextUnformatted("Please copy content flash code");
    CherryGUI::Spacing();

    CherryGUI::SetNextItemWidth(CherryGUI::GetContentRegionAvail().x - 130.0f);
    CherryGUI::InputTextWithHint(
        "##content_flashlink_input", "Paste flashlink here...", input_buffer, IM_ARRAYSIZE(input_buffer));

    CherryGUI::SameLine();

    if (CherryGUI::Button("Search", ImVec2(80.0f, 0.0f))) {
      bool ok = tryProcessCandidate(std::string(input_buffer), false);
      if (!ok) {
        manual_search_error_ = "Invalid flashlink code. Please copy a valid content flashlink.";
      }
    }

    CherryGUI::SameLine();

    if (CherryKit::ButtonImage(Cherry::GetPath("resources/imgs/icons/misc/icon_lightning.png"))
            .GetDataAs<bool>("isClicked")) {
      const char *clipboard = CherryGUI::GetClipboardText();
      bool ok = false;
      if (clipboard != nullptr) {
        ok = tryProcessCandidate(std::string(clipboard), true);
      }
      if (!ok) {
        manual_search_error_ = "No valid content flashlink found in clipboard.";
      }
    }

    if (!manual_search_error_.empty()) {
      CherryGUI::Spacing();
      CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.55f, 0.55f, 1.0f));
      CherryGUI::TextUnformatted(manual_search_error_.c_str());
      CherryGUI::PopStyleColor();
    }

    CherryGUI::Spacing();
    CherryGUI::Separator();
    CherryGUI::Spacing();

    FlashLinkState current_state = state.load();

    switch (current_state) {
      case FlashLinkState::WaitingForClipboard: {
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        CherryGUI::TextUnformatted("Enter a content flashlink above and click Search.");
        CherryGUI::PopStyleColor();
        break;
      }

      case FlashLinkState::Loading: {
        static const char *frames[] = { "|", "/", "-", "\\" };
        int frame = static_cast<int>(CherryGUI::GetTime() / 0.15f) % 4;

        CherryGUI::Text("Searching %s", frames[frame]);
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
        CherryGUI::Text("Flashlink : %s", current_flashlink.c_str());
        CherryGUI::PopStyleColor();
        break;
      }

      case FlashLinkState::Error: {
        std::string error_msg;
        {
          std::lock_guard<std::mutex> lock(result_mutex_);
          error_msg = fetch_result_.error;
        }

        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.45f, 0.45f, 1.0f));
        CherryGUI::TextUnformatted("An error occured");
        CherryGUI::PopStyleColor();
        CherryGUI::TextWrapped("%s", error_msg.c_str());
        break;
      }

      case FlashLinkState::Ready: {
        ContentFetchResult local_result;
        {
          std::lock_guard<std::mutex> lock(result_mutex_);
          local_result = fetch_result_;
        }

        float avail_w = CherryGUI::GetContentRegionAvail().x;

        if (!local_result.info.banner_link.empty()) {
          CherryGUI::Image(Cherry::GetTexture(Cherry::GetHttpPath(local_result.info.banner_link)), ImVec2(avail_w, 120.0f));
          CherryGUI::Spacing();
        }

        CherryGUI::BeginGroup();
        {
          if (!local_result.info.picture_link.empty()) {
            CherryGUI::Image(Cherry::GetTexture(Cherry::GetHttpPath(local_result.info.picture_link)), ImVec2(64.0f, 64.0f));
            CherryGUI::SameLine();
          }

          CherryGUI::BeginGroup();
          CherryGUI::SetWindowFontScale(1.3f);
          CherryGUI::TextUnformatted(
              local_result.info.proper_name.empty() ? "Unknown content" : local_result.info.proper_name.c_str());
          CherryGUI::SetWindowFontScale(1.0f);

          CherryGUI::TextDisabled("%s", local_result.info.name.c_str());
          CherryGUI::EndGroup();
        }
        CherryGUI::EndGroup();

        CherryGUI::Spacing();
        CherryGUI::TextWrapped("%s", local_result.info.description.c_str());

        CherryGUI::Spacing();
        CherryGUI::Separator();
        CherryGUI::Spacing();

        bool has_native = !local_result.compatible_releases.empty();
        bool has_cross = !local_result.compatible_cross_releases.empty();

        if (!has_native && !has_cross) {
          CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
          CherryGUI::TextUnformatted("No compatible version of this content found for your platform.");
          CherryGUI::PopStyleColor();
          break;
        }

        if (!platform_mode_initialized_) {
          selected_platform_mode_ = has_cross ? PlatformTargetMode::Cross : PlatformTargetMode::Native;
          platform_mode_initialized_ = true;
        }

        std::string current_platform = detect_current_platform();
        std::string current_arch = detect_current_arch();
        std::string native_label = native_platform_label(current_platform, current_arch);

        if (has_cross && has_native) {
          CherryGUI::TextUnformatted("Install target :");
          CherryGUI::Spacing();

          bool is_cross = (selected_platform_mode_ == PlatformTargetMode::Cross);

          if (CherryGUI::RadioButton("Cross-platform", is_cross)) {
            selected_platform_mode_ = PlatformTargetMode::Cross;
            selected_release_index_ = 0;
          }
          if (CherryGUI::IsItemHovered()) {
            CherryGUI::SetTooltip(
                "Selecting a cross-platform content will take a bit more disk\n"
                "space, but makes it easy to share the project with other\n"
                "platforms.");
          }

          CherryGUI::SameLine();

          if (CherryGUI::RadioButton(native_label.c_str(), !is_cross)) {
            selected_platform_mode_ = PlatformTargetMode::Native;
            selected_release_index_ = 0;
          }

          CherryGUI::Spacing();
        } else if (has_cross && !has_native) {
          selected_platform_mode_ = PlatformTargetMode::Cross;
          CherryGUI::TextDisabled("Cross-platform content (works on Linux, Windows and macOS)");
          if (CherryGUI::IsItemHovered()) {
            CherryGUI::SetTooltip(
                "This content is cross-platform: it takes a bit more disk\n"
                "space, but makes it easy to share the project with other\n"
                "platforms.");
          }
          CherryGUI::Spacing();
        } else {
          selected_platform_mode_ = PlatformTargetMode::Native;
          CherryGUI::TextDisabled("Platform : %s", native_label.c_str());
          CherryGUI::Spacing();
        }

        const std::vector<ContentRelease> &active_releases = (selected_platform_mode_ == PlatformTargetMode::Cross)
                                                                 ? local_result.compatible_cross_releases
                                                                 : local_result.compatible_releases;

        std::vector<std::string> combo_labels;
        combo_labels.reserve(active_releases.size());
        for (const auto &rel : active_releases) {
          if (selected_platform_mode_ == PlatformTargetMode::Cross) {
            combo_labels.push_back(rel.version);
          } else {
            combo_labels.push_back(rel.version + "  (" + rel.platform + " / " + rel.arch + ")");
          }
        }

        if (selected_release_index_ < 0 || selected_release_index_ >= (int)combo_labels.size()) {
          selected_release_index_ = 0;
        }

        CherryGUI::TextUnformatted("Available versions :");
        if (CherryGUI::BeginCombo("##content_version_combo", combo_labels[selected_release_index_].c_str())) {
          for (int i = 0; i < (int)combo_labels.size(); i++) {
            bool is_selected = (i == selected_release_index_);
            if (CherryGUI::Selectable(combo_labels[i].c_str(), is_selected)) {
              selected_release_index_ = i;
            }
            if (is_selected) {
              CherryGUI::SetItemDefaultFocus();
            }
          }
          CherryGUI::EndCombo();
        }

        CherryGUI::Spacing();
        CherryGUI::Spacing();

        if (CherryGUI::Button("Download and install", ImVec2(avail_w, 0.0f))) {
          const ContentRelease &chosen = active_releases[selected_release_index_];

          std::string parent_uuid = local_result.info.uuid;
          std::string release_url =
              "https://api.infinite.si/api/garagevortex/get_release/content?parent_uuid=" + parent_uuid +
              "&uuid=" + chosen.uuid;
          std::string destination = creation_path_;

          install_progress_ = std::make_shared<vxe::ModuleInstallProgress>();

          auto progress = install_progress_;
          std::thread([release_url, parent_uuid, destination, progress]() {
            std::string body;
            try {
              body = vxe::get_current_context()->net.GET(release_url);
            } catch (...) {
              progress->set_error("Unable to fetch release details.");
              return;
            }

            nlohmann::json release_json;
            try {
              release_json = nlohmann::json::parse(body);
            } catch (...) {
              progress->set_error("Invalid JSON.");
              return;
            }

            vxe::install_content_release_async(release_json, parent_uuid, destination, progress);
          }).detach();
        }

        if (install_progress_) {
          vxe::ModuleInstallState st = install_progress_->state.load();
          CherryGUI::Spacing();
          CherryGUI::Separator();
          CherryGUI::Spacing();

          switch (st) {
            case vxe::ModuleInstallState::Error:
              CherryGUI::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f), "%s", install_progress_->get_error().c_str());
              break;
            case vxe::ModuleInstallState::Done:
              CherryGUI::TextColored(ImVec4(0.4f, 0.9f, 0.4f, 1.0f), "%s", install_progress_->get_status().c_str());
              CherryGUI::TextDisabled("%s", install_progress_->get_install_path().c_str());
              break;
            default: CherryGUI::Text("%s", install_progress_->get_status().c_str()); break;
          }
        }

        break;
      }
    }

    CherryGUI::PopStyleColor();
    CherryGUI::EndChild();
    CherryGUI::PopStyleColor();
  }

  void AddWindow::render_default() {
    float window_width = CherryGUI::GetWindowSize().x;
    float content_width = window_width;
    float main_image_height = window_width / 4.720f;

    CherryGUI::Image(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/banner_add.png")), ImVec2(window_width, main_image_height));

    CherryStyle::AddMarginX(8.0f);
    CherryGUI::Spacing();

    float search_width = content_width - 16.0f;
    CherryGUI::SetCursorPosX((window_width - search_width) * 0.5f);

    CherryNextComponent.SetProperty("size_x", std::to_string(search_width));
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("description", "Search content...");
    CherryNextComponent.SetProperty(
        "description_logo", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
    CherryNextComponent.SetProperty("description_logo_place", "r");
    CherryKit::InputString("", &add_window_search_);

    std::string search = add_window_search_;

    float grid_width = content_width - 22.0f;

    std::vector<TileEntry> quick_tiles = {
      { "addwin_tile_folder",
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add_folder.png")),
        "Folder",
        true,
        [this]() {
          if (create_folder_callback_) {
            create_folder_callback_();
          }
          Cherry::DeleteAppWindow(app_window_);
        } },
      { "addwin_tile_file",
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add_file.png")),
        "File",
        true,
        [this]() {
          if (create_file_callback_) {
            create_file_callback_();
          }
          Cherry::DeleteAppWindow(app_window_);
        } },
    };

    if (vxe::get_current_context()->IO.allow_net) {
      quick_tiles.push_back(
          { "addwin_tile_import",
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/garage2.png")),
            "Garage content",
            true,
            [this]() { show_content_flash_link_ = true; } });
    }

    std::vector<TileEntry> filtered_quick_tiles;
    for (auto &t : quick_tiles) {
      if (MatchesSearch(t.title, search)) {
        filtered_quick_tiles.push_back(t);
      }
    }

    if (!filtered_quick_tiles.empty()) {
      CherryGUI::Spacing();
      CherryStyle::AddMarginX(18.0f);
      CherryKit::SeparatorText("Quick create");
      CherryStyle::AddMarginX(12.0f);
      DrawTileGrid(filtered_quick_tiles, grid_width, 92.0f, 76.0f, 10.0f);
    }

    bool has_module_items = false;
    for (auto m : vxe::get_current_context()->IO.em) {
      if (!m) {
        continue;
      }
      for (auto &ic : m->get_content_browser_item_creators()) {
        if (MatchesSearch(ic->name, search)) {
          has_module_items = true;
          break;
        }
      }
      if (has_module_items) {
        break;
      }
    }

    if (has_module_items) {
      CherryGUI::Spacing();
      CherryStyle::AddMarginX(18.0f);
      CherryKit::SeparatorText("From module");
    }

    for (auto m : vxe::get_current_context()->IO.em) {
      if (!m) {
        continue;
      }

      auto item_creators = m->get_content_browser_item_creators();
      if (item_creators.empty()) {
        continue;
      }

      std::vector<std::shared_ptr<decltype(item_creators)::value_type::element_type>> filtered_creators;
      for (auto &ic : item_creators) {
        if (MatchesSearch(ic->name, search)) {
          filtered_creators.push_back(ic);
        }
      }

      if (filtered_creators.empty()) {
        continue;
      }

      float row_width = content_width - 16.0f;
      CherryStyle::AddMarginX(12.0f);

      CherryNextComponent.SetProperty("size_y", "34.0f");
      CherryNextComponent.SetProperty("padding_y", "1.0f");
      CherryNextComponent.SetProperty("image_size", "14.0f");

      CherryNextComponent.SetProperty("color_border", "#34343466");
      CherryNextComponent.SetProperty("size_x", std::to_string(grid_width - 28.0f));
      CherryNextComponent.SetProperty("color_bg", "#23232300");
      CherryNextComponent.SetProperty("color_bg_hovered", "#34343455");
      CherryNextComponent.SetProperty("color_bg_clicked", "#45454555");
      CherryKit::HeaderImageText(
          m->proper_name(), Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"), [&]() {
            for (auto &ic : filtered_creators) {
              CherryStyle::AddMarginX(12.0f);

              std::string row_id = "addwin_row_" + ic->name;

              ImTextureID icon = Cherry::GetTexture(ic->logo_path);

              CherryNextComponent.SetProperty("size_x", std::to_string(row_width));
              CherryNextComponent.SetProperty("padding_y", "8.0f");
              CherryNextComponent.SetProperty("padding_x", "10.0f");
              CherryNextComponent.SetProperty("rounding", "8.0f");
              CherryNextComponent.SetProperty("color_bg", "#232323");
              CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
              CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
              CherryNextComponent.SetProperty("color_border", "#343434");
              CherryNextComponent.SetProperty("image", icon);
              CherryNextComponent.SetProperty("image_size", "32.0f");

              if (DrawRow(row_id.c_str(), icon, ic->name, ic->description, row_width)) {
                if (ic->create_function && !creation_path_.empty() && create_item_callback_) {
                  create_item_callback_([ic](const std::string &path) { ic->create_function(path); });
                  Cherry::DeleteAppWindow(app_window_);
                }
              }

              CherryGUI::Dummy(ImVec2(0.0f, 4.0f));
            }
          });
    }
  }

  void AddWindow::set_create_file_callback(const std::function<void()> &callback) {
    create_file_callback_ = callback;
  }

  void AddWindow::set_create_folder_callback(const std::function<void()> &callback) {
    create_folder_callback_ = callback;
  }

  void AddWindow::set_import_content_callback(const std::function<void()> &callback) {
    import_content_callback_ = callback;
  }

  void AddWindow::set_create_item_callback(
      const std::function<void(const std::function<void(const std::string &)> &creator)> &callback) {
    create_item_callback_ = callback;
  }

}  // namespace vxe