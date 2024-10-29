#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef CRASH_H
#define CRASH_H

class CrashAppWindow : public std::enable_shared_from_this<CrashAppWindow>
{
public:
	CrashAppWindow(const std::string& name);

	void menubar();
	void addModuleModal();

	void Refresh();
	void Update();

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<CrashAppWindow> Create(const std::string &name);
    void SetupRenderCallback();
    void Render();


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

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
	
	bool CollapseAll = false;
};

#endif // CRASH_H