#pragma once
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#include "../utils.hpp"

#ifndef MODULES_UTILITY_APPWINDOW_H
#define MODULES_UTILITY_APPWINDOW_H

using namespace Cherry;

namespace VortexEditor
{
    struct ModulesUtilityAppWindowChild
    {
        std::string m_Parent;
        std::string m_ChildName;
        std::function<void()> m_Callback;

        ModulesUtilityAppWindowChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child) : m_Parent(parent_name),
                                                                                                                                m_ChildName(child_name),
                                                                                                                                m_Callback(child)
        {
        }
    };

    class ModulesUtilityAppWindow : public std::enable_shared_from_this<ModulesUtilityAppWindow>
    {
    public:
        ModulesUtilityAppWindow(const std::string &name);

        void AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child);
        void RemoveChild(const std::string &child_name);
        std::function<void()> GetChild(const std::string &child_name);
    
        std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
        static std::shared_ptr<ModulesUtilityAppWindow> Create(const std::string &name);
        void SetupRenderCallback();
        void Render();

        std::vector<ModulesUtilityAppWindowChild> m_Childs;

        std::string m_SelectedChildName;

        std::shared_ptr<Cherry::AppWindow> m_AppWindow;
    };
}

#endif // MODULES_UTILITY_APPWINDOW_H