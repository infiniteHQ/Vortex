#include "doc_viewer.hpp"

#include <iostream>

namespace VortexEditor {
DocViewerAppWindow::DocViewerAppWindow(const std::string &name) {
  std::cout << "DocViewerAppWindow" << std::endl;
  m_AppWindow = std::make_shared<AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_journal.png"));
  std::shared_ptr<AppWindow> win = m_AppWindow;
  m_AppWindow->m_Closable = true;
  m_AppWindow->SetCloseCallback(
      [this]() { Cherry::DeleteAppWindow(m_AppWindow); });

  m_AppWindow->SetLeftMenubarCallback([this]() {

  });

  m_AppWindow->SetRightMenubarCallback([this]() {

  });

  m_AppWindow->SetLeftBottombarCallback([this]() {

  });

  this->ctx = VortexMaker::GetCurrentContext();
}

std::shared_ptr<Cherry::AppWindow> &DocViewerAppWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<DocViewerAppWindow>
DocViewerAppWindow::Create(const std::string &name) {
  auto instance =
      std::shared_ptr<DocViewerAppWindow>(new DocViewerAppWindow(name));
  instance->SetupRenderCallback();
  return instance;
}

void DocViewerAppWindow::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void DocViewerAppWindow::Render() { CherryGUI::Text("Hello doc"); }

} // namespace VortexEditor