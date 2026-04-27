#include <templates/delete.h>
#include <vortex.h>
#include <vortex_internals.h>

/**
 * @brief Delete a module of the system
 *
 * @param name Name of the system module.
 * @param version Version of the system module.
 */
VORTEX_API void vxe::DeleteSystemTemplate(const std::string &name, const std::string &version) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  std::vector<std::string> ff;

  for (auto sys_template : ctx->IO.sys_templates) {
    if (sys_template->m_name == name && sys_template->m_version == version) {
      {
        std::string cmd = "rm -rf " + sys_template->m_path;
        std::cout << cmd << std::endl;
        system(cmd.c_str());
      }

      vxe::log_info("Core", "System template nammed \"" + name + "\" with version \"" + version + "\" deleted !");

      return;
    }
  }

  vxe::log_error("Core", "Unable to delete a system template nammed \"" + name + "\" with version \"" + version + "\"");
}