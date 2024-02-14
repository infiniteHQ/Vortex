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
    char name[128];
    //static std::string name;

};

class HostInstance
{
public:
    HostInstance(VxContext *ctx, VxHost* _host);

    // Content Managment 
    void Refresh();
    void Save();

    // UI Managment
    std::string render();
    void close();
    void menubar();

    void UI_ContentWindow();
    void UI_AssetsViewer();
    void UI_ParametersWindow();
    void UI_MainSettings();

    bool opened;
    bool show_UI_ContentWindow = false;
    bool show_UI_ParametersWindow = false;
    bool show_UI_AssetsViewer = false;
    bool show_UI_MainSettings = false;


    std::string name;
    VxContext *m_ctx;
    VxHost* host;
    std::shared_ptr<HostSave> m_currentSave;
    ImGuiID dockspaceID;

    std::shared_ptr<Walnut::Image> m_HostIcon;
    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_SaveIcon;
    std::shared_ptr<Walnut::Image> m_RefreshIcon;
    std::shared_ptr<Walnut::Image> m_DatabaseIcon;
    std::shared_ptr<Walnut::Image> m_EditIcon;
    std::shared_ptr<Walnut::Image> m_SettingsIcon;
    std::shared_ptr<Walnut::Image> m_AddIcon;
    std::shared_ptr<Walnut::Image> m_FolderIcon;
};

#endif // HostInstance_H