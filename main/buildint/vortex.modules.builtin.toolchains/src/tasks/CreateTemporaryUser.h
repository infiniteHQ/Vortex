#include "../toolchain.h"


// TODO : FINISH ALL TASKS INTEGRATIOn
// TODO : TASK LIST 
// TODO : CLEAN

struct CreateTemporaryUser : public Task
{

  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<CreateTemporaryUser>(*this);
  }

  void init() override
  {
    // Taskname
    this->tasktype = "CreateTemporaryUser";

    // Props used by task execution
    this->neededProps.push_back("toolchain");

    // Variables needed by task execution

    // Checklist
    this->addIdleCheck("check_task_deps");
    this->addIdleCheck("group_add_vortex");
    this->addIdleCheck("user_add_vortex");
    this->addIdleCheck("add_vortex_home");
    this->addIdleCheck("give_vortex_home");
    this->addIdleCheck("user_mod_root_to_vortex");
    this->addIdleCheck("implement_bashrc");
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
    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);
    {
      std::string cmd = "groupadd vortex";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("group_add_vortex", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("group_add_vortex", "failed", output, cmd);
    }

    {
      std::string cmd = "useradd -s /bin/bash -g vortex -m -k /dev/null vortex"; // + " -p " + user.vxHostUser_Crypto;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("user_add_vortex", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("user_add_vortex", "failed", output, cmd);
    }

    {
      std::string cmd = "mkdir -pv /home/vortex";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("add_vortex_home", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("add_vortex_home", "failed", output, cmd);
    }

    {
      std::string cmd = "sudo chown -v vortex:vortex  /home/vortex";

      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("give_vortex_home", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("give_vortex_home", "failed", output, cmd);
    }

    {
      std::string cmd = "sudo usermod -aG root vortex";

      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("user_mod_root_to_vortex", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("user_mod_root_to_vortex", "failed", output, cmd);
    }

    this->finish("finish", props);

  }

  void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override
  {
    // Get time
    // Get timer
    // Get all...
    this->stop();

    if (finish_state == "fatal")
    {
      this->state = "fatal";
      return;
    }

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
      std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);
      toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
