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

struct SetupSkeleton : public Task
{

    std::shared_ptr<Task> clone() const override {
        return std::make_shared<SetupSkeleton>(*this);
    }

  void init() override
  {
    this->tasktype = "SetupSkeleton";
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
      this->addCheckVerdict("createFolders", "failed", "To run \"SetupSkeleton\" you need to run \"CreateTemporaryUser\" first");
      this->addCheckVerdict("giveFoldersToUser", "failed", "To run \"SetupSkeleton\" you need to run \"CreateTemporaryUser\" first");
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


    this->addCheckVerdict("createFolders", "success", "Everything is ok");
    std::string baseDir = toolchain->envPath + "/" + toolchain->name;
    if (mkdir(baseDir.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + baseDir);
    }


    std::string crosstoolsDir = baseDir + "/" + toolchain->GetTriplet("target");
    if (mkdir(crosstoolsDir.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + crosstoolsDir);
    }

    toolchain->crosstoolsPath = crosstoolsDir;
    toolchain->workingPath = crosstoolsDir;

    std::string data = baseDir + "/data";
    if (mkdir(data.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + data);
    }

    toolchain->packages_data = baseDir + "/data/packages";
    if (mkdir(toolchain->packages_data.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + toolchain->packages_data);
    }

    std::string patchs_data = baseDir + "/data/patchs";
    if (mkdir(patchs_data.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + patchs_data);
    }

    std::string scripts_data = baseDir + "/data/scripts";
    if (mkdir(scripts_data.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + scripts_data);
    }

    std::string sysrootDir = crosstoolsDir + "/sysroot";
    toolchain->sysrootPath = sysrootDir;
    if (mkdir(sysrootDir.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + sysrootDir);
    }

    std::string debugrootDir = crosstoolsDir + "/debugroot";
    if (mkdir(debugrootDir.c_str(), 0777) == -1)
    {
      this->addCheckVerdict("createFolders", "failed", "Error while creating folder : " + debugrootDir);
    }
    // Give toolchain to user
    std::string cmd = "sudo chown -v -R vortex " + baseDir + "/*";
    if(system((char *)cmd.c_str()) == 0){
      this->addCheckVerdict("giveFoldersToUser", "success", "Everything is ok");
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
