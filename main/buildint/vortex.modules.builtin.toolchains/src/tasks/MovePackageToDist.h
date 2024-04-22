// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../toolchain.h"
struct Task;

struct MovePackageToDist : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<MovePackageToDist>(*this);
  }

  void init() override
  {
    // Taskname
    this->tasktype = "MovePackageToDist";

    // Props used by task execution
    this->neededProps.push_back("toolchain");
    this->neededProps.push_back("package");

    // Variables needed by task execution
    //this->neededVariables.push_back("dist_path:package:[package_name]");

    // Checklist
    this->addIdleCheck("copy_package_to_dist");
    this->addIdleCheck("add_dist_path");
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    if(!this->ifProps(this->neededProps)){this->finish("fatal", nullptr);} // Here


    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr); 
    this->depsChecksSpec.push_back({"toolchain",toolchain->name});
 
    std::shared_ptr<Package> package = this->props->get<std::shared_ptr<Package>>("package", nullptr);
    this->depsChecksSpec.push_back({"package",package->name});
 

    std::tuple<std::string,std::string,std::string> v_packageData = toolchain->currentLoadedSystem.get_varable(this, "directory:data_packages");
    std::string packageData = std::get<2>(v_packageData);



    {
      std::string cmd = "cp -r " + package->path + " " + packageData;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("copy_package_to_dist", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("copy_package_to_dist", "failed", output, cmd);

      size_t posDernierSlash = package->path.find_last_of('/');
      if (posDernierSlash != std::string::npos)
      {
        std::string resultat = package->path.substr(posDernierSlash + 1);
        package->distPath = packageData + "/" + resultat;


        toolchain->currentLoadedSystem.put_varable(this, "dist_path:package:"+package->name+"", "SetupDistEnvironment", package->distPath);

        std::string final = "The package dist output is now : \"" + package->distPath + "\"";

        this->addCheckVerdict("add_dist_path", "success", final, "\"Get the path of master package folder, and add the path of package.\"");
      }
      else{
        this->addCheckVerdict("add_dist_path", "failed", "Error: The package dist output is not found", "\"Get the path of master package folder, and add the path of package.\"");
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
      std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);
      toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
