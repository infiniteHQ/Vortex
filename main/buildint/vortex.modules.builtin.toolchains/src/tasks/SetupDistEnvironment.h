// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

// WARNING: This is a vanilla VortexMaker features, the core and UI can interact directly with it.

/*
    Description :
*/

#include "../toolchain.h"

struct Task;

struct SetupDistEnvironment : public Task
{

    std::shared_ptr<Task> clone() const override {
        return std::make_shared<SetupDistEnvironment>(*this);
    }

  void init() override
  {
    this->tasktype = "SetupDistEnvironment";

    // Props used by task execution
    this->neededProps.push_back("toolchain");

    // Variables needed by task execution

    this->addIdleCheck("create_folder:base");
    this->addIdleCheck("create_folder:package_data");
    this->addIdleCheck("create_folder:patchs_data");
    this->addIdleCheck("create_folder:scripts_data");
    this->addIdleCheck("create_folder:working_host");
    this->addIdleCheck("create_folder:working_host_debugroot");
    this->addIdleCheck("create_folder:working_host_sysroot");
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();


    if(!this->ifProps(this->neededProps)){
      this->finish("failed", nullptr);
    }
    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);



    // API to check if a task is executed and the result.
    if(!toolchain->TaskSuccedded("CreateTemporaryUser")){
      std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
      this->addCheckVerdict("createFolders", "failed", "To run \"SetupDistEnvironment\" you need to run \"CreateTemporaryUser\" first", "none");
      this->addCheckVerdict("giveFoldersToUser", "failed", "To run \"SetupDistEnvironment\" you need to run \"CreateTemporaryUser\" first", "none");
      this->finish("failed", props);
      return;
    }

    // Plutot : créer un nouveau report de cette tache

    // Set steps flags

    toolchain->envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;

    toolchain->targetTriplet = toolchain->GetTriplet("target");
    toolchain->builderTriplet = toolchain->GetTriplet("builder");
    toolchain->hostTriplet = toolchain->GetTriplet("host");


    this->addCheckVerdict("createFolders", "success", "Everything is ok", "none");
    std::string baseDir = toolchain->envPath + "/" + toolchain->name;


    toolchain->currentLoadedSystem.put_varable(this, "directory:base", "SetupDistEnvironment", baseDir);
    if (mkdir(baseDir.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + baseDir, "none");
    }


    std::string crosstoolsDir = baseDir + "/working_host";
    toolchain->currentLoadedSystem.put_varable(this, "directory:working_host", "SetupDistEnvironment", crosstoolsDir);
    if (mkdir(crosstoolsDir.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + crosstoolsDir, "none");
    }

    // All provided by default. Used by the current toolchain system
    {
      std::string path = baseDir + "/data";
      toolchain->currentLoadedSystem.put_varable(this, "directory:data", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + baseDir;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:data", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:data", "failed", output, cmd);
    }


    {
      std::string path = baseDir + "/data/packages";
      toolchain->currentLoadedSystem.put_varable(this, "directory:data_packages", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + path;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:packages_data", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:packages_data", "failed", output, cmd);
    }

    {
      std::string path = baseDir + "/data/scripts";
      toolchain->currentLoadedSystem.put_varable(this, "directory:data_scripts", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + path;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:packages_data", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:packages_data", "failed", output, cmd);
    }


    {
      std::string path = baseDir + "/data/patchs";
      toolchain->currentLoadedSystem.put_varable(this, "directory:data_scripts", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + path;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:patchs_data", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:patchs_data", "failed", output, cmd);
    }



    {
      std::string path = baseDir + "/data/patchs";
      toolchain->currentLoadedSystem.put_varable(this, "directory:data_scripts", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + path;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:patchs_data", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:patchs_data", "failed", output, cmd);
    }


    {
      std::string path = crosstoolsDir + "/usr";
      toolchain->currentLoadedSystem.put_varable(this, "directory:sysroot", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + path;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:working_host_sysroot", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:working_host_sysroot", "failed", output, cmd);
    }


    {
      std::string path = crosstoolsDir + "/usr/include";
      toolchain->currentLoadedSystem.put_varable(this, "directory:sysroot", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + path;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:working_host_sysroot", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:working_host_sysroot", "failed", output, cmd);
    }

    {
      std::string path = crosstoolsDir + "/sysroot";
      toolchain->currentLoadedSystem.put_varable(this, "directory:sysroot", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + path;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:working_host_sysroot", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:working_host_sysroot", "failed", output, cmd);
    }

    {
      std::string path = crosstoolsDir + "/debugroot";
      toolchain->currentLoadedSystem.put_varable(this, "directory:debugroot", "SetupDistEnvironment", path);

      std::string cmd = "mkdir " + path;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)this->addCheckVerdict("create_folder:working_host_debugroot", "success", output, cmd);
      if (result != 0)this->addCheckVerdict("create_folder:working_host_debugroot", "failed", output, cmd);
    }



      toolchain->currentLoadedSystem.put_varable(this, "toolchain:target_triplet", "SetupDistEnvironment", toolchain->GetTriplet("target"));
      toolchain->currentLoadedSystem.put_varable(this, "toolchain:build_triplet", "SetupDistEnvironment", toolchain->GetTriplet("builder"));
      toolchain->currentLoadedSystem.put_varable(this, "toolchain:host_triplet", "SetupDistEnvironment", toolchain->GetTriplet("host"));

    this->finish("success", props);
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
