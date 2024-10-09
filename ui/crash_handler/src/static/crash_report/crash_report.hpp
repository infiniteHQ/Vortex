#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef CRASH_H
#define CRASH_H

class Crash
{
public:
	Crash(const std::string& name);

	void menubar();
	void addModuleModal();

	void Refresh();
	void Update();

        void RefreshRender(const std::shared_ptr<Crash>& instance);

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow()
        {
            return m_AppWindow;
        }

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