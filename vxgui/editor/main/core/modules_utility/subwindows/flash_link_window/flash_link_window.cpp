//
//  flash_link_window.cpp
//  Sources for the "flash link window" of the modules utility
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./flash_link_window.hpp"

#include <algorithm>
#include <cstdio>
#include <sstream>
#include <thread>

namespace vxe {

  namespace {

    struct Palette {
      const char* card;
      const char* border;
      const char* text;
      const char* sub;
      const char* field;
      const char* neutral;
      const char* neutralHover;
      const char* accent;
      const char* accentHover;
      const char* accentText;
      const char* danger;
      const char* warn;
    };

    Palette get_palette() {
      if (CherryApp.GetTheme() == "dark") {
        return { "#232323", "#333333", "#FFFFFF", "#8A8A8A", "#1B1B1B", "#2E2E2E",
                 "#3A3A3A", "#B1FF31", "#C3FF53", "#121212", "#EE5555", "#EEAA55" };
      }
      return { "#FFFFFF", "#D8D8D8", "#232323", "#6B6B6B", "#F1F1F1", "#E6E6E6",
               "#D2D2D2", "#3E8E00", "#4FA800", "#FFFFFF", "#D32F2F", "#C77700" };
    }

    ImVec4 col(const char* hex) {
      return Cherry::HexToRGBA(hex);
    }

    void colored_text(const char* hex, const std::string& text) {
      ImGui::PushStyleColor(ImGuiCol_Text, col(hex));
      ImGui::TextUnformatted(text.c_str());
      ImGui::PopStyleColor();
    }

    void wrapped_text(const char* hex, const std::string& text, float wrap_x) {
      ImGui::PushTextWrapPos(wrap_x);
      colored_text(hex, text);
      ImGui::PopTextWrapPos();
    }

    void centered_text(const char* hex, const std::string& text) {
      float w = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((std::max)(0.0f, (ImGui::GetWindowWidth() - w) * 0.5f));
      colored_text(hex, text);
    }

    bool action_button(const char* label, const char* bg, const char* hover, const char* fg, float width, float height) {
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
      ImGui::PushStyleColor(ImGuiCol_Button, col(bg));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col(hover));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, col(hover));
      ImGui::PushStyleColor(ImGuiCol_Text, col(fg));
      bool clicked = ImGui::Button(label, ImVec2(width, height));
      ImGui::PopStyleColor(4);
      ImGui::PopStyleVar();
      return clicked;
    }

    void pill(const std::string& text, const char* bg, const char* fg) {
      ImVec2 ts = ImGui::CalcTextSize(text.c_str());
      ImVec2 size(ts.x + 16.0f, ts.y + 6.0f);
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImDrawList* dl = ImGui::GetWindowDrawList();
      dl->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), Cherry::HexToImU32(bg), size.y * 0.5f);
      dl->AddText(ImVec2(p.x + 8.0f, p.y + 3.0f), Cherry::HexToImU32(fg), text.c_str());
      ImGui::Dummy(size);
    }

    void spinner(float radius, const char* hex) {
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImVec2 c(p.x + radius, p.y + radius);
      float a = (float)ImGui::GetTime() * 6.0f;
      ImDrawList* dl = ImGui::GetWindowDrawList();
      dl->PathClear();
      dl->PathArcTo(c, radius, a, a + 4.5f, 24);
      dl->PathStroke(Cherry::HexToImU32(hex), 0, 3.0f);
      ImGui::Dummy(ImVec2(radius * 2.0f, radius * 2.0f));
    }

    template<class Texture>
    void rounded_image(Texture texture, ImVec2 size, float rounding) {
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImGui::GetWindowDrawList()->AddImageRounded(
          texture, p, ImVec2(p.x + size.x, p.y + size.y), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), IM_COL32_WHITE, rounding);
      ImGui::Dummy(size);
    }

    struct CardScope {
      ImDrawList* draw_list = nullptr;
      ImVec2 origin;
      float width = 0.0f;
      float pad = 16.0f;
      float inner_w = 0.0f;
      float wrap_x = 0.0f;
    };

    CardScope begin_card() {
      CardScope c;
      c.draw_list = ImGui::GetWindowDrawList();
      c.width = ImGui::GetContentRegionAvail().x;
      c.inner_w = c.width - c.pad * 2.0f;
      c.wrap_x = ImGui::GetCursorPosX() + c.width - c.pad;
      c.origin = ImGui::GetCursorScreenPos();
      c.draw_list->ChannelsSplit(2);
      c.draw_list->ChannelsSetCurrent(1);
      ImGui::BeginGroup();
      ImGui::Dummy(ImVec2(0.0f, c.pad));
      ImGui::Indent(c.pad);
      return c;
    }

    void end_card(const CardScope& c, const Palette& pal) {
      ImGui::Unindent(c.pad);
      ImGui::Dummy(ImVec2(0.0f, c.pad));
      ImGui::EndGroup();
      ImVec2 max(c.origin.x + c.width, ImGui::GetItemRectMax().y);
      c.draw_list->ChannelsSetCurrent(0);
      c.draw_list->AddRectFilled(c.origin, max, Cherry::HexToImU32(pal.card), 14.0f);
      c.draw_list->AddRect(c.origin, max, Cherry::HexToImU32(pal.border), 14.0f);
      c.draw_list->ChannelsMerge();
    }

    void push_field_style(const Palette& pal) {
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 9.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, col(pal.field));
      ImGui::PushStyleColor(ImGuiCol_Border, col(pal.border));
      ImGui::PushStyleColor(ImGuiCol_Text, col(pal.text));
      ImGui::PushStyleColor(ImGuiCol_PopupBg, col(pal.card));
    }

    void pop_field_style() {
      ImGui::PopStyleColor(4);
      ImGui::PopStyleVar(3);
    }

    void separator(const Palette& pal) {
      ImGui::PushStyleColor(ImGuiCol_Separator, col(pal.border));
      ImGui::Separator();
      ImGui::PopStyleColor();
    }

    bool radio_button(const Palette& pal, const char* label, bool active) {
      ImGui::PushStyleColor(ImGuiCol_CheckMark, col(pal.accent));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, col(pal.field));
      ImGui::PushStyleColor(ImGuiCol_Text, col(pal.text));
      bool clicked = ImGui::RadioButton(label, active);
      ImGui::PopStyleColor(3);
      return clicked;
    }

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

    std::vector<int> parse_version_numbers(const std::string& v) {
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

    bool version_greater(const std::string& a, const std::string& b) {
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

    std::string extract_major(const std::string& version, int keep = 2) {
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

    std::string native_platform_label(const std::string& platform, const std::string& arch) {
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

  ModuleFetchResult fetch_module_from_flashlink(const std::string& flashlink) {
    ModuleFetchResult result;

    if (vxe::get_current_context()->disconnected) {
      result.error = "No internet connexion";
      return result;
    }

    const std::string prefix = "mod:";
    if (flashlink.rfind(prefix, 0) != 0) {
      result.error = "Invalid flashlink";
      return result;
    }
    std::string uuid = flashlink.substr(prefix.size());

    auto& ctx = *vxe::get_current_context();

    std::string module_url = "http://api.infinite.si:9000/api/garagevortex/get_module?uuid=" + uuid;
    std::string module_body;
    try {
      module_body = ctx.net.GET(module_url);
    } catch (...) {
      result.error = "Unable to fetch the service API";
      return result;
    }

    if (module_body.empty()) {
      result.error = "Empty response from the API";
      return result;
    }

    nlohmann::json module_json;
    try {
      module_json = nlohmann::json::parse(module_body);
    } catch (...) {
      result.error = "Invalid JSON";
      return result;
    }

    ModuleInfo info;
    info.uuid = module_json.value("uuid", "");
    info.name = module_json.value("name", "");
    info.proper_name = module_json.value("proper_name", "");
    info.description = module_json.value("description", "");
    info.picture_link = module_json.value("picture_link", "");
    info.banner_link = module_json.value("banner_link", "");
    info.host = module_json.value("host", "");
    info.host_link = module_json.value("host_link", "");
    info.certified = module_json.value("certified", false);
    info.state = module_json.value("state", "");

    result.info = info;

    std::string releases_url =
        "https://api.infinite.si/api/garagevortex/get_content_releases_summary?parent_uuid=" + uuid + "&kind=module";

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

    std::vector<ModuleRelease> parsed;

    for (auto& item : releases_json["releases"]) {
      if (!item.is_object()) {
        continue;
      }

      ModuleRelease rel;
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

    std::vector<ModuleRelease> cross;
    for (auto& rel : parsed) {
      if (rel.platform == "cross") {
        cross.push_back(rel);
      }
    }
    std::sort(cross.begin(), cross.end(), [](const ModuleRelease& a, const ModuleRelease& b) {
      return version_greater(a.version, b.version);
    });

    std::vector<ModuleRelease> compatible;
    for (auto& rel : parsed) {
      if (rel.platform == current_platform && rel.arch == current_arch && rel.major == current_major) {
        compatible.push_back(rel);
      }
    }

    if (compatible.empty()) {
      for (auto& rel : parsed) {
        if (rel.platform == current_platform && rel.arch == current_arch) {
          compatible.push_back(rel);
        }
      }
    }

    std::sort(compatible.begin(), compatible.end(), [](const ModuleRelease& a, const ModuleRelease& b) {
      return version_greater(a.version, b.version);
    });

    result.compatible_releases = compatible;
    result.compatible_cross_releases = cross;
    result.success = true;
    return result;
  }

  FlashLinkWindow::FlashLinkWindow(const std::string& name, const std::string& mode) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [this]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(8.0f);
    app_window_->SetInternalPaddingY(8.0f);

    mode_ = mode == "flash" ? "flash" : "prompt";
  }

  std::shared_ptr<Cherry::AppWindow>& FlashLinkWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<FlashLinkWindow> FlashLinkWindow::create(const std::string& name, const std::string& mode) {
    auto instance = std::shared_ptr<FlashLinkWindow>(new FlashLinkWindow(name, mode));
    instance->setup_render_callback();
    return instance;
  }

  void FlashLinkWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void FlashLinkWindow::startSearch(const std::string& flashlink) {
    uint64_t my_token = ++search_token;

    manual_search_error_.clear();
    current_flashlink = flashlink;
    selected_release_index_ = 0;
    platform_mode_initialized_ = false;
    install_progress_.reset();
    state = FlashLinkState::Loading;

    std::thread([this, my_token, flashlink]() {
      ModuleFetchResult res = vxe::fetch_module_from_flashlink(flashlink);

      if (search_token.load() != my_token) {
        return;
      }

      {
        std::lock_guard<std::mutex> lock(result_mutex_);
        fetch_result_ = res;
      }

      state = res.success ? FlashLinkState::Ready : FlashLinkState::Error;
    }).detach();
  }

  bool FlashLinkWindow::tryProcessCandidate(const std::string& raw, bool clear_input_on_success) {
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

    if (!is_module_flashlink(decoded)) {
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

  void FlashLinkWindow::renderCloseButton() {
    const Palette pal = get_palette();

    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    separator(pal);
    ImGui::Dummy(ImVec2(0.0f, 6.0f));

    if (action_button("Close", pal.neutral, pal.neutralHover, pal.text, ImGui::GetContentRegionAvail().x, 36.0f)) {
      Cherry::DeleteAppWindow(app_window_);
    }
  }

  void FlashLinkWindow::render() {
    const Palette pal = get_palette();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 12.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::BeginChild("##flash_module", ImVec2(0.0f, 0.0f), true);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    Cherry::PushFont("ClashBold");
    colored_text(pal.text, "Install from a FlashLink");
    Cherry::PopFont();
    colored_text(pal.sub, "Install a module shared with a FlashLink.");
    ImGui::Dummy(ImVec2(0.0f, 8.0f));

    if (mode_ == "flash") {
      if (!clipboard_checked) {
        clipboard_checked = true;

        const char* clipboard = ImGui::GetClipboardText();
        if (clipboard != nullptr) {
          tryProcessCandidate(std::string(clipboard), false);
        }
      }
    } else {
      const float paste_w = 76.0f;
      const float search_w = 88.0f;
      const float gap = ImGui::GetStyle().ItemSpacing.x;
      const float row_h = ImGui::GetFontSize() + 18.0f;

      push_field_style(pal);
      ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - paste_w - search_w - gap * 2.0f);
      bool submitted = ImGui::InputTextWithHint(
          "##flashlink_input",
          "Paste your flashlink code here...",
          input_buffer,
          IM_ARRAYSIZE(input_buffer),
          ImGuiInputTextFlags_EnterReturnsTrue);
      pop_field_style();

      ImGui::SameLine();
      if (action_button("Paste", pal.neutral, pal.neutralHover, pal.text, paste_w, row_h)) {
        const char* clipboard = ImGui::GetClipboardText();
        if (clipboard) {
          std::snprintf(input_buffer, IM_ARRAYSIZE(input_buffer), "%s", clipboard);
        }
      }
      ImGui::SameLine();
      if (action_button("Search", pal.accent, pal.accentHover, pal.accentText, search_w, row_h) || submitted) {
        bool ok = tryProcessCandidate(std::string(input_buffer), false);
        manual_search_error_ = ok ? "" : "Invalid flashlink code. Please copy a valid module flashlink.";
      }

      if (!manual_search_error_.empty()) {
        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        colored_text(pal.danger, manual_search_error_);
      }
    }

    ImGui::Dummy(ImVec2(0.0f, 6.0f));
    separator(pal);
    ImGui::Dummy(ImVec2(0.0f, 8.0f));

    FlashLinkState current_state = state.load();

    switch (current_state) {
      case FlashLinkState::WaitingForClipboard: {
        ImGui::Dummy(ImVec2(0.0f, 28.0f));
        centered_text(pal.sub, mode_ == "flash" ? "Click on a FlashLink icon first." : "Enter a code above.");
        ImGui::Dummy(ImVec2(0.0f, 28.0f));
        break;
      }

      case FlashLinkState::Loading: {
        const float radius = 14.0f;
        ImGui::Dummy(ImVec2(0.0f, 24.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - radius * 2.0f) * 0.5f);
        spinner(radius, pal.accent);
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        centered_text(pal.sub, "Searching...");
        centered_text(pal.sub, current_flashlink);
        ImGui::Dummy(ImVec2(0.0f, 24.0f));
        break;
      }

      case FlashLinkState::Error: {
        std::string error_msg;
        {
          std::lock_guard<std::mutex> lock(result_mutex_);
          error_msg = fetch_result_.error;
        }

        CardScope card = begin_card();
        Cherry::PushFont("ClashBold");
        colored_text(pal.danger, "An error occurred");
        Cherry::PopFont();
        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        wrapped_text(pal.text, error_msg, card.wrap_x);
        end_card(card, pal);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        break;
      }

      case FlashLinkState::Ready: {
        ModuleFetchResult local_result;
        {
          std::lock_guard<std::mutex> lock(result_mutex_);
          local_result = fetch_result_;
        }

        float avail_w = ImGui::GetContentRegionAvail().x;

        if (!local_result.info.banner_link.empty()) {
          rounded_image(
              Cherry::GetTexture(Cherry::GetHttpPath(local_result.info.banner_link)), ImVec2(avail_w, 120.0f), 14.0f);
          ImGui::Dummy(ImVec2(0.0f, 8.0f));
        }

        CardScope info = begin_card();
        if (!local_result.info.picture_link.empty()) {
          rounded_image(
              Cherry::GetTexture(Cherry::GetHttpPath(local_result.info.picture_link)), ImVec2(64.0f, 64.0f), 14.0f);
          ImGui::SameLine(0.0f, 14.0f);
        }
        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        Cherry::PushFont("ClashBold");
        colored_text(pal.text, local_result.info.proper_name.empty() ? "Unknown module" : local_result.info.proper_name);
        Cherry::PopFont();
        pill("Module", pal.accent, pal.accentText);
        ImGui::SameLine(0.0f, 8.0f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
        colored_text(pal.sub, local_result.info.name);
        ImGui::EndGroup();

        if (!local_result.info.description.empty()) {
          ImGui::Dummy(ImVec2(0.0f, 8.0f));
          wrapped_text(pal.sub, local_result.info.description, info.wrap_x);
        }
        end_card(info, pal);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        bool has_native = !local_result.compatible_releases.empty();
        bool has_cross = !local_result.compatible_cross_releases.empty();

        CardScope install = begin_card();

        if (!has_native && !has_cross) {
          wrapped_text(pal.warn, "No compatible version of this module found for your platform.", install.wrap_x);
          end_card(install, pal);
          ImGui::Dummy(ImVec2(0.0f, 10.0f));
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
          colored_text(pal.sub, "Install target");

          bool is_cross = (selected_platform_mode_ == PlatformTargetMode::Cross);

          if (radio_button(pal, "Cross-platform", is_cross)) {
            selected_platform_mode_ = PlatformTargetMode::Cross;
            selected_release_index_ = 0;
          }
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(
                "Selecting a cross-platform module will take a bit more disk\n"
                "space, but makes it easy to share the project with other\n"
                "platforms.");
          }

          ImGui::SameLine();

          if (radio_button(pal, native_label.c_str(), !is_cross)) {
            selected_platform_mode_ = PlatformTargetMode::Native;
            selected_release_index_ = 0;
          }
        } else if (has_cross && !has_native) {
          selected_platform_mode_ = PlatformTargetMode::Cross;
          colored_text(pal.sub, "Cross-platform module (works on Linux, Windows and macOS)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(
                "This module is cross-platform: it takes a bit more disk\n"
                "space, but makes it easy to share the project with other\n"
                "platforms.");
          }
        } else {
          selected_platform_mode_ = PlatformTargetMode::Native;
          colored_text(pal.sub, "Platform : " + native_label);
        }

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        const std::vector<ModuleRelease>& active_releases = (selected_platform_mode_ == PlatformTargetMode::Cross)
                                                                ? local_result.compatible_cross_releases
                                                                : local_result.compatible_releases;

        std::vector<std::string> combo_labels;
        combo_labels.reserve(active_releases.size());
        for (const auto& rel : active_releases) {
          if (selected_platform_mode_ == PlatformTargetMode::Cross) {
            combo_labels.push_back(rel.version);
          } else {
            combo_labels.push_back(rel.version + "  (" + rel.platform + " / " + rel.arch + ")");
          }
        }

        if (selected_release_index_ < 0 || selected_release_index_ >= (int)combo_labels.size()) {
          selected_release_index_ = 0;
        }

        colored_text(pal.sub, "Available versions");
        push_field_style(pal);
        ImGui::SetNextItemWidth(install.inner_w);
        if (ImGui::BeginCombo("##version_combo", combo_labels[selected_release_index_].c_str())) {
          for (int i = 0; i < (int)combo_labels.size(); i++) {
            bool is_selected = (i == selected_release_index_);
            if (ImGui::Selectable(combo_labels[i].c_str(), is_selected)) {
              selected_release_index_ = i;
            }
            if (is_selected) {
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }
        pop_field_style();

        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        if (action_button("Download and install", pal.accent, pal.accentHover, pal.accentText, install.inner_w, 40.0f)) {
          const ModuleRelease& chosen = active_releases[selected_release_index_];

          std::string parent_uuid = local_result.info.uuid;
          std::string release_url =
              "https://api.infinite.si/api/garagevortex/get_release/module?parent_uuid=" + parent_uuid +
              "&uuid=" + chosen.uuid;

          install_progress_ = std::make_shared<vxe::ModuleInstallProgress>();

          auto progress = install_progress_;
          std::thread([release_url, parent_uuid, progress]() {
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

            vxe::install_module_release_async(release_json, parent_uuid, progress);
          }).detach();
        }

        if (install_progress_) {
          vxe::ModuleInstallState st = install_progress_->state.load();
          ImGui::Dummy(ImVec2(0.0f, 6.0f));

          switch (st) {
            case vxe::ModuleInstallState::Error:
              wrapped_text(pal.danger, install_progress_->get_error(), install.wrap_x);
              break;
            case vxe::ModuleInstallState::Done:
              wrapped_text(pal.accent, install_progress_->get_status(), install.wrap_x);
              wrapped_text(pal.sub, install_progress_->get_install_path(), install.wrap_x);
              break;
            default:
              spinner(8.0f, pal.accent);
              ImGui::SameLine(0.0f, 8.0f);
              wrapped_text(pal.sub, install_progress_->get_status(), install.wrap_x);
              break;
          }
        }

        end_card(install, pal);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        break;
      }
    }

    renderCloseButton();
    ImGui::EndChild();
  }

}  // namespace vxe