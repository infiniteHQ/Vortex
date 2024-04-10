// UI (with imgui)
#include "../../backend/Source/editor/Application.h"
#include "../../backend/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"
#include "../../vendor/imgui/imgui.h"
#include "../../vendor/imgui/imgui_internal.h"
#include "../../vendor/stb-image/stb_image.h"

#include "../../backend/Assets/icons.h"

#include "../../../../main/include/vortex.h"


#include "../../../src/instanceFactory.h"
#include "../../../instances/Utils/TextEditor/TextEditorInstance.h"

#ifndef ScriptInstance_H
#define ScriptInstance_H

using namespace VortexMaker;
class InstanceFactory;



class ScriptInstance
{
public:
    ScriptInstance(VxContext *ctx, std::shared_ptr<VxScript> script, InstanceFactory* factory);

    // Content Managment 
    void Refresh();
    void Save();

    // UI Managment
    std::string render();
    void close();
    void menubar();

    void UI_MainSettings();
    void UI_FileBrowser();


  void SpawnInstance(std::shared_ptr<TextEditorInstance> instance);
  void UnspawnInstance(std::shared_ptr<TextEditorInstance> instance);

    bool opened;
    bool show_UI_MainSettings = false;
    bool show_UI_CompilationArguments = false;
    bool show_UI_ActionsEditor = false;
    bool show_UI_FileBrowser = false;
    bool show_UI_DirectoryTreeView = true;
    bool show_UI_DirectoryFinder = false;

    std::string name;
    VxContext *m_ctx;
    std::shared_ptr<VxScript> script;
    std::shared_ptr<VxScriptSave> m_currentSave;
    ImGuiID dockspaceID;
    std::shared_ptr<TextEditor> editor;
    InstanceFactory* factory;


    std::vector<std::shared_ptr<TextEditorInstance>> textInstances;

    std::shared_ptr<Walnut::Image> m_ScriptIcon;
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

#endif // ScriptInstance_H