#include "../../../../main/include/vortex.h"
#include "../../../../main/include/vortex_internals.h"
#include "../../../../lib/uikit/uikit.h"

#ifndef LOGUTILITY_LAUNCHER_H
#define LOGUTILITY_LAUNCHER_H

class LauncherLogUtility
{
public:
	LauncherLogUtility(VxContext *_ctx, const std::string& parent);

	void OnImGuiRender(const std::string& parent, std::function<void(ImGuiWindow*)> controller);
	void menubar();

	VxContext *ctx;

private:
	bool opened;
	std::string parent;
	std::shared_ptr<VxToolchain> latest_toolchain;

	std::shared_ptr<UIKit::Image> m_JournalIcon;
	std::shared_ptr<UIKit::Image> m_ListIcon;
	std::shared_ptr<UIKit::Image> m_RefreshIcon;
	std::shared_ptr<UIKit::Image> m_AddIcon;
};

#endif // LOGUTILITY_LAUNCHER_H
