
#include "../../../../lib/uikit/Source/editor/Application.h"
#include "../../../../lib/uikit/Platform/GUI/editor/ImGui/ImGuiTheme.h"
#include "../../../../lib/uikit/Platform/GUI/editor/Image.h"
#include "../../../../lib/uikit/Platform/GUI/editor/UI/UI.h"
#include "../../../../lib/uikit/Assets/icons.h"
#include "../src/instanceFactory.h"

class ModuleManager
{
public:
	ModuleManager(VxContext *_ctx, InstanceFactory* _factory);

	void OnImGuiRender();
	void menubar();


	VxContext *ctx;
	InstanceFactory* factory;

private:
	bool opened;

	std::shared_ptr<VxToolchain> latest_toolchain;

    std::shared_ptr<Walnut::Image> m_ListIcon;
    std::shared_ptr<Walnut::Image> m_RefreshIcon;
    std::shared_ptr<Walnut::Image> m_AddIcon;
    std::shared_ptr<Walnut::Image> m_TrashIcon;

	bool CollapseAll = false;
};
