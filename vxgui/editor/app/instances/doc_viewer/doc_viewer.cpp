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

void DocViewerAppWindow::Render() {
  std::string markdownText = R"(
#H1 Header : Text and Links
You can add [links like this one to enkisoftware](https://www.enkisoftware.com/)
and lines will wrap well. You can also insert images ![image alt text](image
identifier e.g. filename) Horizontal rules:
***
___
*Emphasis* and **strong emphasis** change the appearance of the text.
## H2 Header: indented text.
  This text has an indent (two leading spaces).
    This one has two.
### H3 Header: Lists
  * Unordered lists
    * Lists can be indented with two extra spaces.
  * Lists can have [links like this one to Avoyd](https://www.avoyd.com/) and
*emphasized text*
)";

  ImGui::MarkdownView(markdownText);
}

} // namespace VortexEditor