
#include "../../../../backend/Source/editor/Application.h"
#include "../../../../backend/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../../../backend/Platform/GUI/editor/Image.h"
#include "../../../../backend/Platform/GUI/editor/UI/UI.h"
#include "../../../../backend/Assets/icons.h"
#include "../../../src/instanceFactory.h"
#include "../../../../../../main/include/vortex.h"

#ifndef PackageInstance_H
#define PackageInstance_H

using namespace VortexMaker;
class InstanceFactory;


class PackageInstance
{
public:
    PackageInstance(VxContext *ctx, std::shared_ptr<VxPackage> _package);

    // Content Managment 
    void Refresh();
    void Save();

    // UI Managment
    std::string render();
    void close();
    void menubar();

    void UI_MainSettings();
    void UI_CompilationArguments();
    void UI_ActionsEditor();

    bool opened;
    bool show_UI_MainSettings = false;
    bool show_UI_CompilationArguments = false;
    bool show_UI_ActionsEditor = false;


    std::string name;
    VxContext *m_ctx;
    std::shared_ptr<VxPackage> package;
    std::shared_ptr<PackageSave> m_currentSave;
    ImGuiID dockspaceID;

    std::shared_ptr<Walnut::Image> m_HostIcon;
    std::shared_ptr<Walnut::Image> m_Icon;
    std::shared_ptr<Walnut::Image> m_SaveIcon;
    std::shared_ptr<Walnut::Image> m_RefreshIcon;
    std::shared_ptr<Walnut::Image> m_DatabaseIcon;
    std::shared_ptr<Walnut::Image> m_TrashIcon;
    std::shared_ptr<Walnut::Image> m_PackageIcon;
    std::shared_ptr<Walnut::Image> m_EditIcon;
    std::shared_ptr<Walnut::Image> m_BuildIcon;
    std::shared_ptr<Walnut::Image> m_SettingsIcon;
    std::shared_ptr<Walnut::Image> m_AddIcon;
    std::shared_ptr<Walnut::Image> m_FolderIcon;
};

#endif // PackageInstance_H