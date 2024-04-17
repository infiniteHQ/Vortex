#include "../../../../../../lib/uikit/uikit.h"
#include "../../../../../../main/include/vortex.h"
#include "../../../assets/icons.h"
#include "../../../../../../main/include/vortex_internals.h"
#include "../../module.h"

#ifndef PackageModulesInstance_H
#define PackageModulesInstance_H

using namespace VortexMaker;
class InstanceFactory;


class PackageRenderInstance : public ModuleRenderInstance
{
public:
    PackageRenderInstance(VxContext *ctx, std::shared_ptr<Package> _package);

    // Content Managment 
    void Refresh();
    void Save();

    // UI Managment
    void render() override;
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
    std::shared_ptr<Package> package;
    std::shared_ptr<PackageSave_> m_currentSave;
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

#endif // PackageModulesInstance_H