// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../../../../src/vortex.h"
#include "../../../../src/vortex_internals.h"

struct Task;

struct ClearDistPackage : public Task
{
    std::shared_ptr<Task> clone() const override {
        return std::make_shared<ClearDistPackage>(*this);
    }

  void init() override
  {
    this->tasktype = "ClearDistPackage";
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);

    this->addIdleCheck("delete_vortex_user");
    {
      std::string cmd = "userdel -r vortex";
      int result = system((char *)cmd.c_str());

      if(result == 0) this->addCheckVerdict("delete_vortex_user", "success", "Vortex user deleted succefully !", cmd);
      if(result != 0) this->addCheckVerdict("delete_vortex_user", "failed", "Vortex user failed to be deleted !", cmd);
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
      std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
        toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
