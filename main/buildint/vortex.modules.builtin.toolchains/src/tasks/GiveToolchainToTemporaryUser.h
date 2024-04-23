// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../toolchain.h"

struct Task;

struct GiveToolchainToTemporaryUser : public Task
{
    std::shared_ptr<Task> clone() const override {
        return std::make_shared<GiveToolchainToTemporaryUser>(*this);
    }

  void init() override
  {
    this->tasktype = "GiveToolchainToTemporaryUser";

    this->neededProps.push_back("toolchain");

    this->addIdleCheck("give_vortex_package_data");
    this->addIdleCheck("give_vortex_working_dir");
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();


    if(!this->ifProps(this->neededProps)){this->finish("fatal", nullptr);} // Here
    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);    
      
    std::tuple<std::string,std::string,std::string> v_packageData = toolchain->currentLoadedSystem.get_varable(this, "directory:data_packages");
    std::string packageData = std::get<2>(v_packageData);


    std::string PackageDataPath;
    PackageDataPath += ctx->projectPath;
    PackageDataPath += "/./.vx/dist/toolchains/" + toolchain->name + "/data";

    std::string WorkingDirPath;
    WorkingDirPath += ctx->projectPath;
    WorkingDirPath += "/./.vx/dist/toolchains/" + toolchain->name + "/working_host";

    {
      std::string cmd = "sudo chown -v -R vortex:vortex " + packageData;
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
      std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);
        toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
