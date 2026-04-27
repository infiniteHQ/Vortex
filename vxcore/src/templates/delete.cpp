//
//  delete.cpp
//  Sources for template deletions features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex.h>
#include <vortex_internals.h>

#include <templates/delete.hpp>

VORTEX_API void vxe::delete_system_template(const std::string &name, const std::string &version) {
  auto ctx = vxe::get_current_context();

  std::vector<std::string> ff;

  for (auto sys_template : ctx->IO.sys_templates) {
    if (sys_template->name_ == name && sys_template->version_ == version) {
      {
        std::string cmd = "rm -rf " + sys_template->path_;
        std::cout << cmd << std::endl;
        system(cmd.c_str());
      }

      vxe::log_info("Core", "System template nammed \"" + name + "\" with version \"" + version + "\" deleted !");

      return;
    }
  }

  vxe::log_error("Core", "Unable to delete a system template nammed \"" + name + "\" with version \"" + version + "\"");
}