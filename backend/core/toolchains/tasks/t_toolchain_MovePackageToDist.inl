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

struct MovePackageToDist : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<MovePackageToDist>(*this);
  }

  void init() override
  {
    this->tasktype = "MovePackageToDist";
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
    std::shared_ptr<VxPackage> package = this->props->get<std::shared_ptr<VxPackage>>("package", nullptr);

    this->addIdleCheck("copy_package_to_dist");
    this->addIdleCheck("add_dist_path");

    std::string PackageDataPath;
    PackageDataPath += ctx->projectPath;
    PackageDataPath += "/./.vx/dist/toolchains/" + toolchain->name + "/data/packages/";

    {
      std::string cmd = "cp -r " + package->path + " " + PackageDataPath;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("copy_package_to_dist", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("copy_package_to_dist", "failed", output, cmd);

      size_t posDernierSlash = package->path.find_last_of('/');
      if (posDernierSlash != std::string::npos)
      {
        std::string resultat = package->path.substr(posDernierSlash + 1);
        package->distPath = PackageDataPath + "/" + resultat;
        std::string final = "The package dist output is now : \"" + package->distPath + "\"";

        this->addCheckVerdict("copy_package_to_dist", "success", final, "\"Get the path of master package folder, and add the path of package.\"");
      }
      else{
        this->addCheckVerdict("copy_package_to_dist", "failed", "Error: The package dist output is not found", "\"Get the path of master package folder, and add the path of package.\"");
      }
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
      std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
      toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};