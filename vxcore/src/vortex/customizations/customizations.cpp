#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

void vxe::push_edit_menu_item(
    const std::string &title,
    const std::function<void()> &action,
    const std::string &logo,
    const std::string &section) {
  auto ctx = vxe::get_current_context();
  ctx->editMenuItems.push_back({ title, action, logo, section });
}

void vxe::pop_edit_menu_item(const int &count) {
  if (count <= 0)
    return;

  auto ctx = vxe::get_current_context();
  const int remove = (std::min)(count, static_cast<int>(ctx->editMenuItems.size()));
  ctx->editMenuItems.erase(ctx->editMenuItems.end() - remove, ctx->editMenuItems.end());
}

void vxe::clear_edit_menu_item() {
  auto ctx = vxe::get_current_context();
  ctx->editMenuItems.clear();
}

void vxe::push_custom_menu(const std::string &title, const std::function<void()> &render) {
  auto ctx = vxe::get_current_context();
  ctx->customMenus.push_back({ title, render });
}

void vxe::pop_custom_menu(const int &count) {
  if (count <= 0)
    return;

  auto ctx = vxe::get_current_context();
  const int remove = (std::min)(count, static_cast<int>(ctx->customMenus.size()));
  ctx->customMenus.erase(ctx->customMenus.end() - remove, ctx->customMenus.end());
}

void vxe::clear_custom_menus() {
  auto ctx = vxe::get_current_context();
  ctx->customMenus.clear();
}