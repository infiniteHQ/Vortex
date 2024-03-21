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

struct CompileDistPackage : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<CompileDistPackage>(*this);
  }

  void init() override
  {
    this->tasktype = "CompileDistPackage";
  };


  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();



    if(!this->ifProps({"package", "toolchain"})){
      this->finish("failed", nullptr);
    }

    std::shared_ptr<VxPackage> package = this->props->get<std::shared_ptr<VxPackage>>("package", nullptr);
    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);

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
    this->addIdleCheck("configure_dist_package");
    this->addIdleCheck("compile_dist_package");
    this->addIdleCheck("install_dist_package");


    package->ExecuteActions("pre_compilation", package);

    {
    std::string compilation;
    if (package->compilation.exclusiveCustomConfigProcess == "not specified")
    {

      // Configure
      if (package->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        std::string cmd = "";

        cmd += "sudo -u vortex -i sh -c 'cd " + working_path + +"/build && ";

        for (auto suffix : package->compilation.compilationPrefixes)
        {
          if (suffix.first == "all" || suffix.first == toolchain->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

      if (package->compilation.useCompilationOptimization)
      {
        cmd += " make -j6";
      }
      else
      {
        cmd += " make";
      }

        for (auto parameter : package->compilation.compilationSuffixes)
        {
          if (parameter.first == "all" || parameter.first == toolchain->target_arch)
          {
            cmd += parameter.second + " ";
          }
        }

        compilation = VortexMaker::replacePlaceholders(cmd, replacements);
      }
      else
      {
        compilation += "sudo -u vortex -i sh -c 'cd " + working_path + "/build && ";
        compilation += VortexMaker::replacePlaceholders(package->compilation.exclusiveCustomConfigProcess, replacements);
      }
    }


      auto [output, result] = toolchain->exec_cmd_quote(compilation.c_str()); // faire un empaquetage complet

      if (result == 0)this->addCheckVerdict("compile_dist_package", "success", output, compilation);
      if (result != 0)this->addCheckVerdict("compile_dist_package", "failed", output, compilation);
    }

    package->ExecuteActions("post_compilation", package);
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
