// UI (with imgui)
#include "../../backend/Source/editor/Application.h"
#include "../../backend/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"
#include "../../vendor/imgui/imgui.h"
#include "../../vendor/stb-image/stb_image.h"

#include "../../backend/Assets/icons.h"

#include "../../../../vortex.h"

#ifndef HostInstance_H
#define HostInstance_H

using namespace VortexMaker;


struct HostSave{
    std::string name;
    std::string author;
    std::string type;
    std::string state;
    std::string vendor;
    std::string platform;
    std::string host_arch;
    std::string target_arch;
};

class HostInstance
{
public:
    HostInstance(VxContext *ctx, VxHost* _host);

    std::string render();
    void close();

    void menubar();

    void UI_ContentWindow();
    void UI_AssetsViewer();
    void UI_ParametersWindow();

    bool opened;
    bool show_UI_ContentWindow = false;
    bool show_UI_ParametersWindow = false;
    bool show_UI_AssetsViewer = false;


    std::string name;
    VxContext *m_ctx;
    VxHost* host;
    ImGuiID dockspaceID;

    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_SaveIcon;
    std::shared_ptr<Walnut::Image> m_SettingsIcon;
    std::shared_ptr<Walnut::Image> m_AddIcon;
    std::shared_ptr<Walnut::Image> m_FolderIcon;
};

#endif // HostInstance_H