// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../toolchain.h"

struct Task;

struct CreateBuildEnv : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<CreateBuildEnv>(*this);
  }

  void init() override
  {
    this->tasktype = "CreateBuildEnv";

    // Props used by task execution
    this->neededProps.push_back("package");
    this->neededProps.push_back("toolchain");

    // Variables needed by task execution
    this->neededVariables.push_back("dist_path:package_uncompressed:[package_name]");

    // Checklist
    this->addIdleCheck("create_build_folder");
  };


  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();


    if(!this->ifProps(this->neededProps)){this->finish("fatal", nullptr);}

    std::shared_ptr<Package> package = this->props->get<std::shared_ptr<Package>>("package", nullptr);
    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);


    std::string working_path = std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "dist_path:package_uncompressed:"+package->name + ""));

    std::unordered_map<std::string, std::string> replacements = {
        {"${Target}", toolchain->targetTriplet},
        {"${Build}", toolchain->builderTriplet},
        {"${Host}", toolchain->hostTriplet},
        {"${WorkingHost}", std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "directory:working_host"))},
        {"${Sysroot}", toolchain->sysrootPath},
        {"${PackageFolder}", package->path},
        {"${DistPackageFolder}", working_path}};

    this->addIdleCheck("create_build_folder");

    {
      std::string cmd = "mkdir " + working_path + +"/build";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str()); // faire un empaquetage complet

      if (result == 0)this->addCheckVerdict("create_build_folder", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_build_folder", "failed", output, cmd);
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
