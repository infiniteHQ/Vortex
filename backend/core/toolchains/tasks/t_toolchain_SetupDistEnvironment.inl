// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

// WARNING: This is a vanilla VortexMaker features, the core and UI can interact directly with it.

/*
    Description :
*/

#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"

struct Task;

struct SetupDistEnvironment : public Task
{

    std::shared_ptr<Task> clone() const override {
        return std::make_shared<SetupDistEnvironment>(*this);
    }

  void init() override
  {
    this->tasktype = "SetupDistEnvironment";
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);

    this->addIdleCheck("createFolders");
    this->addIdleCheck("giveFoldersToUser");

    // API to check if a task is executed and the result.
    if(!toolchain->TaskSuccedded("CreateTemporaryUser")){
      std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
      this->addCheckVerdict("createFolders", "failed", "To run \"SetupDistEnvironment\" you need to run \"CreateTemporaryUser\" first", "none");
      this->addCheckVerdict("giveFoldersToUser", "failed", "To run \"SetupDistEnvironment\" you need to run \"CreateTemporaryUser\" first", "none");
      this->finish("failed", props);
      return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
    std::string data = baseDir + "/data";
    toolchain->currentLoadedSystem.put_varable(this, "directory:data", "SetupDistEnvironment", baseDir);
    if (mkdir(data.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + data, "none");
    }

    std::string packages_data = baseDir + "/data/packages";
    toolchain->currentLoadedSystem.put_varable(this, "directory:data_packages", "SetupDistEnvironment", packages_data);
    if (mkdir(toolchain->packages_data.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + toolchain->packages_data, "none");
    }

    std::string patchs_data = baseDir + "/data/patchs";
    toolchain->currentLoadedSystem.put_varable(this, "directory:data_patchs", "SetupDistEnvironment", patchs_data);
    if (mkdir(patchs_data.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + patchs_data, "none");
    }

    std::string scripts_data = baseDir + "/data/scripts";
    toolchain->currentLoadedSystem.put_varable(this, "directory:scripts_patchs", "SetupDistEnvironment", scripts_data);
    if (mkdir(scripts_data.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + scripts_data, "none");
    }

    std::string sysrootDir = crosstoolsDir + "/sysroot";
    toolchain->currentLoadedSystem.put_varable(this, "directory:sysroot", "SetupDistEnvironment", sysrootDir);
    if (mkdir(sysrootDir.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + sysrootDir, "none");
    }

    std::string debugrootDir = crosstoolsDir + "/debugroot";
    toolchain->currentLoadedSystem.put_varable(this, "directory:debugroot", "SetupDistEnvironment", debugrootDir);
    if (mkdir(debugrootDir.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + debugrootDir, "none");
    }
    // Give toolchain to user
    std::string cmd = "sudo chown -v -R vortex " + baseDir + "/*";
    if(system((char *)cmd.c_str()) == 0){
      this->addCheckVerdict("giveFoldersToUser", "success", "Everything is ok", "none");
    }

    this->finish("deps_error", props);
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
