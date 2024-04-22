// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../toolchain.h"

struct Task;

struct DeleteTemporaryUser : public Task
{
    std::shared_ptr<Task> clone() const override {
        return std::make_shared<DeleteTemporaryUser>(*this);
    }

  void init() override
  {
    this->tasktype = "DeleteTemporaryUser";
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);

    

    this->addIdleCheck("delete_vortex_user");

    {
      std::string cmd = "userdel -r vortex";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());


      if(result == 0) this->addCheckVerdict("delete_vortex_user", "success", output, cmd);
      if(result != 0) this->addCheckVerdict("delete_vortex_user", "failed", output, cmd);
    }
    
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
    if(this->successCounter > 0){
      this->state = "success";
    }
    
    if(this->warningCounter > 0){
      this->state = "warning";
    }

    if(this->failCounter > 0){
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
