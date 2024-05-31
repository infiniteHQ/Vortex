#include "../../../../main/include/vortex.h"
#include "../../../../main/include/vortex_internals.h"
#include "../../../../lib/uikit/uikit.h"


#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

class SystemSettings
{
public:
	SystemSettings(VxContext *_ctx);

	void OnImGuiRender();
	void menubar();
	void addModuleModal();

	void Refresh();
	void Update();

	/**
	 * @brief Menu items
	*/
	void mainButtonsMenuItem();
	void filterMenuItem();
	void createMenuItem();
	void searchMenuItem();

	VxContext *ctx;

private:
	bool opened;

	std::shared_ptr<VxToolchain> latest_toolchain;

    std::thread receiveThread;
	std::vector<std::shared_ptr<TemplateInterface>> project_templates;
    std::shared_ptr<UIKit::Image> m_ListIcon;
    std::shared_ptr<UIKit::Image> m_RefreshIcon;
    std::shared_ptr<UIKit::Image> m_OpenIcon;
    std::shared_ptr<UIKit::Image> m_ProjectIcon;
    std::shared_ptr<UIKit::Image> m_SearchIcon;
    std::shared_ptr<UIKit::Image> m_AddIcon;
    std::shared_ptr<UIKit::Image> m_TrashIcon;
	bool CollapseAll = false;
};

#endif // SYSTEMSETTINGS_H