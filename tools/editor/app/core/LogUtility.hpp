
// #include "../instances/Components/Host/HostInstance.h"
// #include "../instances/Components/Toolchain/ToolchainInstance.h"

#ifndef LOGUTILITY_H
#define LOGUTILITY_H

#include "../include/instanceFactory.h"

class LogUtility
{
public:
	LogUtility(VxContext *_ctx, InstanceFactory *_factory);

	void OnImGuiRender();
	void menubar();

	VxContext *ctx;
	InstanceFactory *factory;

private:
	bool opened;
	std::shared_ptr<VxToolchain> latest_toolchain;

	std::shared_ptr<UIKit::Image> m_JournalIcon;
	std::shared_ptr<UIKit::Image> m_ListIcon;
	std::shared_ptr<UIKit::Image> m_RefreshIcon;
	std::shared_ptr<UIKit::Image> m_AddIcon;
};

#endif // LOGUTILITY_H
