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

struct UncompressDistPackage : public Task
{
    std::shared_ptr<Task> clone() const override {
        return std::make_shared<UncompressDistPackage>(*this);
    }

  void init() override
  {
    this->tasktype = "UncompressDistPackage";
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
    std::shared_ptr<VxPackage> package = this->props->get<std::shared_ptr<VxPackage>>("package", nullptr);



    this->addIdleCheck("uncompress");
    this->addIdleCheck("set_final_path");

    // API to check if a task is executed and the result.
    if(!toolchain->TaskSuccedded("MovePackageToDist")){
      std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
      this->addCheckVerdict("uncompress", "failed", "To run \"UncompressDistPackage\" you need to run \"MovePackageToDist\" first", "none");
      this->addCheckVerdict("set_final_path", "failed", "To run \"UncompressDistPackage\" you need to run \"MovePackageToDist\" first", "none");
      this->finish("failed", props);
      return;
    }


    std::string path;

    {
      std::string cmd = "cd " + package->distPath + " && sudo tar -xvf " + package->fileName;
      
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if(result == 0) this->addCheckVerdict("uncompress", "success", output, cmd);
      if(result != 0) this->addCheckVerdict("uncompress", "failed", output, cmd);
    }



    size_t lastSlashPos = package->fileName.find_last_of('/');
    if (lastSlashPos != std::string::npos)
    {
      std::string foldername = package->fileName.substr(lastSlashPos + 1);

      size_t extensionPos = foldername.rfind(".tar.xz");
      if (extensionPos != std::string::npos)
      {
        foldername.erase(extensionPos);
      }
      size_t extensionPosSecond = foldername.rfind(".tar.gz");
      if (extensionPosSecond != std::string::npos)
      {
        foldername.erase(extensionPosSecond);
      }
      size_t extensionPosThird = foldername.rfind(".tar.bz2");
      if (extensionPosThird != std::string::npos)
      {
        foldername.erase(extensionPosThird);
      }

      package->path = foldername;
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
