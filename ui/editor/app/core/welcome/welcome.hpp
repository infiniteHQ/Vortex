#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef WELCOME_WINDOW_H
#define WELCOME_WINDOW_H

using namespace Cherry;

namespace VortexEditor
{
    class WelcomeWindow : public std::enable_shared_from_this<WelcomeWindow>
    {
    public:
        WelcomeWindow(const std::string &name);

        void AddChild(const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);

        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<WelcomeWindow> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();

        std::unordered_map<std::string, std::function<void()>> m_Childs;

        std::string m_SelectedChildName;

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };
}

#endif // WELCOME_WINDOW_H