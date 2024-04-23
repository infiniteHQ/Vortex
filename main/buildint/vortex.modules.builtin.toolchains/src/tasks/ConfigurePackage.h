#include "../toolchain.h"


struct ConfigurePackage : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<ConfigurePackage>(*this);
  }

  void init() override
  {
    this->tasktype = "ConfigurePackage";

    // Props used by task execution
    this->neededProps.push_back("package");
    this->neededProps.push_back("toolchain");

    // Variables needed by task execution
    //this->neededVariables.push_back("dist_path:package:\[package_name\]");

    // Checklist

    this->addIdleCheck("find_package");
    this->addIdleCheck("exec_pre_configuration");
    this->addIdleCheck("configure_dist_package");
    this->addIdleCheck("exec_post_configuration");
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

    if(package != nullptr){
      this->addCheckVerdict("find_package", "success", "none", "Seems to be ok.");
    }

    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);
    this->depsChecksSpec.push_back({"toolchain",toolchain->name});

    std::string working_path = std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "dist_path:package_uncompressed:"+package->name + ""));

    std::shared_ptr<Task> t = std::make_shared<Task>(*this);
    package->allTasks.push_back(t);
    package->latestTask = t;

    // API to check if a task is executed and the result.
    if(!toolchain->TaskSuccedded("CreateTemporaryUser")){
      std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
      this->addCheckVerdict("createFolders", "failed", "To run \"SetupDistEnvironment\" you need to run \"CreateTemporaryUser\" first", "none");
      this->addCheckVerdict("giveFoldersToUser", "failed", "To run \"SetupDistEnvironment\" you need to run \"CreateTemporaryUser\" first", "none");
      this->finish("failed", props);
      return;
    }

    std::unordered_map<std::string, std::string> replacements = {
        {"${Target}",std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "toolchain:target_triplet"))},
        {"${Build}",std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "toolchain:build_triplet"))},
        {"${Host}", std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "toolchain:host_triplet"))},
        {"${WorkingHost}", std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "directory:working_host"))},
        {"${Sysroot}", toolchain->sysrootPath},
        {"${PackageFolder}", package->path},
        {"${DistPackageFolder}", working_path}};

    /*
        Faire pleins de taches singulières (config, give etc, et faire des plus grosses taches qui controllent et qui call les petites taches)
    
    */
    package->distPath = working_path;
    package->ExecuteActions("pre_configuration", package);
    this->addCheckVerdict("exec_pre_configuration", "success", "none", "Seems to be ok.");


    {
    std::string configuration;
    if (package->compilation.exclusiveCustomConfigProcess == "not specified")
    {

      // Configure
      if (package->compilation.exclusiveCustomConfigProcess == "not specified")
      {
        std::string cmd = "";

        cmd += "sudo -u vortex -i sh -c 'cd " + working_path + +"/build && ";

        for (auto suffix : package->compilation.configurationPrefixes)
        {
          if (suffix.first == "all" || suffix.first == toolchain->target_arch)
          {
            cmd += suffix.second + " ";
          }
        }

        cmd += "../configure ";

        for (auto parameter : package->compilation.configurationSuffixes)
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
        configuration += "sudo -u vortex -i sh -c 'cd " + working_path + "/build && ";
        configuration += VortexMaker::replacePlaceholders(package->compilation.exclusiveCustomConfigProcess, replacements);
      }
    }
    else
      {
        configuration += "sudo -u vortex -i sh -c 'cd " + working_path + "/build && ";
        configuration += VortexMaker::replacePlaceholders(package->compilation.exclusiveCustomConfigProcess, replacements);
      }



      auto [output, result] = toolchain->exec_cmd_quote(configuration.c_str()); // faire un empaquetage complet

      if (result == 0)this->addCheckVerdict("configure_dist_package", "success", output, configuration);
      if (result != 0)this->addCheckVerdict("configure_dist_package", "failed", output, configuration);
    }


    package->ExecuteActions("post_configuration", package);
    this->addCheckVerdict("exec_post_configuration", "success", "none", "Seems to be ok.");

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
