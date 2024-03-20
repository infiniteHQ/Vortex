// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"

struct Task;

struct GiveToolchainToTemporaryUser : public Task
{
    std::shared_ptr<Task> clone() const override {
        return std::make_shared<GiveToolchainToTemporaryUser>(*this);
    }

  void init() override
  {
    this->tasktype = "GiveToolchainToTemporaryUser";
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);

    this->addIdleCheck("give_vortex_package_data");
    this->addIdleCheck("give_vortex_working_dir");


    std::string PackageDataPath;
    PackageDataPath += ctx->projectPath;
    PackageDataPath += "/./.vx/dist/toolchains/" + toolchain->name + "/data";

    std::string WorkingDirPath;
    WorkingDirPath += ctx->projectPath;
    WorkingDirPath += "/./.vx/dist/toolchains/" + toolchain->name + "/working_host";

    {
      std::string cmd = "sudo chown -v -R vortex:vortex " + PackageDataPath;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());
    
      if(0 == 0) this->addCheckVerdict("give_vortex_package_data", "success", "Vortex user deleted succefully !", cmd);
      if(0 != 0) this->addCheckVerdict("give_vortex_package_data", "failed", "Vortex user failed to be deleted !", cmd);
    }

    {
      std::string cmd = "sudo chown -v -R vortex:vortex " + WorkingDirPath;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());
    
      if(0 == 0) this->addCheckVerdict("give_vortex_working_dir", "success", "Vortex user deleted succefully !", cmd);
      if(0 != 0) this->addCheckVerdict("give_vortex_working_dir", "failed", "Vortex user failed to be deleted !", cmd);
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
