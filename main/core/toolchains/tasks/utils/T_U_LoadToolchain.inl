// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../../../../main/include/vortex.h"
#include "../../../../main/include/vortex_internals.h"

struct Task;

struct T_U_LoadToolchain : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<T_U_LoadToolchain>(*this);
  }

  void init() override
  {
    this->tasktype = "T_U_LoadToolchain";

    // Props used by task execution
    // this->neededProps.push_back("package");
    this->neededProps.push_back("toolchain");

    this->addIdleCheck("create_bashrc");
    this->addIdleCheck("echo_bashrc_cc");
    this->addIdleCheck("echo_bashrc_cxx");
    this->addIdleCheck("echo_bashrc_ar");
    this->addIdleCheck("echo_bashrc_as");
    this->addIdleCheck("echo_bashrc_ld");
    this->addIdleCheck("echo_bashrc_ranlib");
    this->addIdleCheck("echo_bashrc_strip");
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    if (!this->ifProps(this->neededProps))
    {
      this->finish("fatal", nullptr);
    }

    std::vector<std::pair<std::string, std::string>> env_props = this->props->get<std::vector<std::pair<std::string, std::string>>>("env_props", {{"null", "null"}});
    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);

    // Need CreateUser


    std::unordered_map<std::string, std::string> replacements = {
        {"${WorkingHost}", std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "directory:working_host"))},
    };


    {
      std::string cmd = "sudo touch /home/vortex/.bashrc";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("create_bashrc", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("create_bashrc", "failed", output, cmd);
    }


    {
      std::string cmd = "sudo echo \"CC='" + toolchain->distToolchain.CC + "'\" >> /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_cc", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_cc", "failed", output, cmd);
    }


    {
      std::string cmd = "sudo echo \"export CC='" + toolchain->distToolchain.CC + "'\" >> /home/vortex/.bash_profile";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_cc", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_cc", "failed", output, cmd);
    }



    {
      std::string cmd = "sudo echo \"CXX='" + toolchain->distToolchain.CXX + "'\" >> /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_cxx", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_cxx", "failed", output, cmd);
    }



    {
      std::string cmd = "sudo echo \"AR='" + toolchain->distToolchain.AR + "'\" >> /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_ar", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_ar", "failed", output, cmd);
    }



    {
      std::string cmd = "sudo echo \"AS='" + toolchain->distToolchain.AS + "'\" >> /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_as", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_as", "failed", output, cmd);
    }



    {
      std::string cmd = "sudo echo \"RANLIB='" + toolchain->distToolchain.RANLIB + "'\" >> /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_ranlib", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_ranlib", "failed", output, cmd);
    }



    {
      std::string cmd = "sudo echo \"STRIP='" + toolchain->distToolchain.STRIP + "'\" >> /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_strip", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_strip", "failed", output, cmd);
    }



    {
      std::string cmd = "sudo echo \"LD='" + toolchain->distToolchain.LD + "'\" >> /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_ld", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_ld", "failed", output, cmd);
    }

    {
      std::string cmd = "source /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_ld", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_ld", "failed", output, cmd);
    }
    {
      std::string cmd = "source /home/vortex/.bash_profile";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_ld", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_ld", "failed", output, cmd);
    }
    {
      std::string cmd = "sudo -u vortex -i sh -c 'source /home/vortex/.bash_profile";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd_quote(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_cc", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_cc", "failed", output, cmd);
    }
    {
      std::string cmd = "sudo -u vortex -i sh -c 'source /home/vortex/.bashrc";
      cmd = VortexMaker::replacePlaceholders(cmd, replacements);
      auto [output, result] = toolchain->exec_cmd_quote(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("echo_bashrc_cc", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("echo_bashrc_cc", "failed", output, cmd);
    }


    this->addCheckVerdict("finished", "success", "none", "none");
    this->finish("finish", nullptr);
  }

  void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override
  {
    // Get time
    // Get timer
    // Get all...
    this->stop();
    this->result_props = result_properties;

    this->state = "unknow";
    if (this->successCounter > 0)
    {
      this->state = "success";
    }

    if (this->warningCounter > 0)
    {
      this->state = "warning";
    }

    if (this->failCounter > 0)
    {
      this->state = "failed";
    }

    if (this->props)
    {
      std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
      toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
