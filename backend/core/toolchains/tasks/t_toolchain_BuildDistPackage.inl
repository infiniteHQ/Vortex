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

struct BuildDistPackage : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<BuildDistPackage>(*this);
  }

  void init() override
  {
    this->tasktype = "BuildDistPackage";
  };



// Pour demain :
/*
  - Finir toutes les tasks de toolchain

*/






  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);

    std::shared_ptr<VxPackage> package = this->props->get<std::shared_ptr<VxPackage>>("package", nullptr);

    std::unordered_map<std::string, std::string> replacements = {
        {"${Target}", toolchain->targetTriplet},
        {"${Build}", toolchain->builderTriplet},
        {"${Host}", toolchain->hostTriplet},
        {"${Crosstools}", toolchain->crosstoolsPath},
        {"${Sysroot}", toolchain->sysrootPath},
        {"${PackageFolder}", package->path},
        {"${DistPackageFolder}", package->distPath}};

    this->addIdleCheck("configure_dist_package");
    this->addIdleCheck("compile_dist_package");
    this->addIdleCheck("install_dist_package");

    std::string configuration;
    if (package->compilation.exclusiveCustomConfigProcess == "not specified")
    {

      // Configure
      std::string configuration;
      if (package->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        std::string cmd = "";

        cmd += "sudo -u vortex -i sh -c 'cd " + package->distPath + +"/build && ";

        for (auto suffix : package->compilation.configurationSuffixes)
        {
          if (suffix.first == "all" || suffix.first == toolchain->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += "../configure ";

        for (auto parameter : package->compilation.configurationPrefixes)
        {
          if (parameter.first == "all" || parameter.first == toolchain->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        configuration = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        configuration += "sudo -u vortex -i sh -c 'cd " + package->distPath + "/build && ";
        configuration += VortexMaker::replacePlaceholders(package->compilation.exclusiveCustomConfigProcess, replacements);
      }
    }

    {
      std::string cmd = "userdel -r vortex";

      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("configure_dist_package", "success", "Vortex user deleted succefully !", cmd);
      if (result != 0)this->addCheckVerdict("configure_dist_package", "failed", "Vortex user failed to be deleted !", cmd);
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
