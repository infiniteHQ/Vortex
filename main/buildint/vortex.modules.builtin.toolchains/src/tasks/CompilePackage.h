#include "../toolchain.h"

/**
 * @brief This task is used to compile a package with make or with a custom method.
 * Corresponding asset : "Package" with conventionnal identifier "P_".
 * 
 * @author Diego E. Moreno <d@infinite.si>
 * @name CompilePackage
*/
struct CompilePackage : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<CompilePackage>(*this);
  }

  void init() override
  {
    this->tasktype = "CompilePackage";

    // Props used by task execution
    this->neededProps.push_back("package");
    this->neededProps.push_back("toolchain");


    this->neededVariables.push_back("dist_path:package_uncompressed:[package_name]");

    this->addIdleCheck("find_package");
    this->addIdleCheck("exec_pre_compilation");
    this->addIdleCheck("compile_dist_package");
    this->addIdleCheck("exec_post_compilation");


  };


  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();


    if(!this->ifProps(this->neededProps)){this->finish("fatal", nullptr);}

    std::shared_ptr<Package> package = this->props->get<std::shared_ptr<Package>>("package", nullptr);
    this->depsChecksSpec.push_back({"package",package->name});

    std::shared_ptr<Task> t = std::make_shared<Task>(*this);
    package->allTasks.push_back(t);
    package->latestTask = t;

    if(package != nullptr){
      this->addCheckVerdict("find_package", "success", "none", "Seems to be ok.");
    }

    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);
    this->depsChecksSpec.push_back({"toolchain",toolchain->name});


    std::tuple<std::string,std::string,std::string> v_packageData = toolchain->currentLoadedSystem.get_varable(this, "dist_path:package_uncompressed:"+package->name + "");
    std::string working_path = std::get<2>(v_packageData);

    std::unordered_map<std::string, std::string> replacements = {
        {"${Target}", std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "toolchain:target_triplet"))},
        {"${Build}", std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "toolchain:build_triplet"))},
        {"${Host}", std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "toolchain:host_triplet"))},
        {"${WorkingHost}", std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "directory:working_host"))},
        {"${Sysroot}", toolchain->sysrootPath},
        {"${PackageFolder}", package->path},
        {"${DistPackageFolder}", working_path}};

    package->ExecuteActions("pre_compilation", package);
    this->addCheckVerdict("exec_pre_compilation", "success", "none", "Seems to be ok.");

    {
    std::string compilation;
    if (package->compilation.exclusiveCustomCompilationProcess == "not specified")
    {

      // Configure
      if (package->compilation.exclusiveCustomCompilationProcess == "not specified")
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
        cmd += " make -j6 ";
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
        compilation += VortexMaker::replacePlaceholders(package->compilation.exclusiveCustomCompilationProcess, replacements);
      }
    }      
    else
      {
        compilation += "sudo -u vortex -i sh -c 'cd " + working_path + "/build && ";
        compilation += VortexMaker::replacePlaceholders(package->compilation.exclusiveCustomCompilationProcess, replacements);
      }


      auto [output, result] = toolchain->exec_cmd_quote(compilation.c_str()); // faire un empaquetage complet

      if (result == 0)this->addCheckVerdict("compile_dist_package", "success", output, compilation);
      if (result != 0)this->addCheckVerdict("compile_dist_package", "failed", output, compilation);
    }

    package->ExecuteActions("post_compilation", package);
    this->addCheckVerdict("exec_post_compilation", "success", "none", "Seems to be ok.");

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
