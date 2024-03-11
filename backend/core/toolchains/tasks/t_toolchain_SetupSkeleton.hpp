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

struct SetupSkeleton : public Task
{
  void init() override
  {
    this->tasktype = "SetupSkeleton";
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<hArgs> props;
    std::shared_ptr<VxToolchain> toolchain = props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);

    // Plutot : créer un nouveau report de cette tache

    // Set steps flags
    this->addIdleCheck("deployement");
    this->addIdleCheck("preparation");
    this->addIdleCheck("decompression");
    this->addIdleCheck("configuration");
    this->addIdleCheck("compilation");
    this->addIdleCheck("installation");
    this->addIdleCheck("post_process");

    toolchain->envPath = ctx->projectPath / ctx->paths.toolchainDistFolder;

    toolchain->targetTriplet = toolchain->GetTriplet("target");
    toolchain->builderTriplet = toolchain->GetTriplet("builder");
    toolchain->hostTriplet = toolchain->GetTriplet("host");

    std::string baseDir = toolchain->envPath + "/" + toolchain->name;
    if (mkdir(baseDir.c_str(), 0777) == -1)
    {
      perror("Erreur lors de la création du dossier baseDir");
    }


    std::string crosstoolsDir = baseDir + "/" + toolchain->GetTriplet("target");
    if (mkdir(crosstoolsDir.c_str(), 0777) == -1)
    {
      perror("Erreur lors de la création du dossier crosstoolsDir");
    }

    toolchain->crosstoolsPath = crosstoolsDir;
    toolchain->workingPath = crosstoolsDir;

    std::string data = baseDir + "/data";
    if (mkdir(data.c_str(), 0777) == -1)
    {
      perror("Erreur lors de la création du dossier crosstoolsDir");
    }

    toolchain->packages_data = baseDir + "/data/packages";
    if (mkdir(toolchain->packages_data.c_str(), 0777) == -1)
    {
      perror("Erreur lors de la création du dossier crosstoolsDir");
    }

    std::string patchs_data = baseDir + "/data/patchs";
    if (mkdir(patchs_data.c_str(), 0777) == -1)
    {
      perror("Erreur lors de la création du dossier crosstoolsDir");
    }

    std::string scripts_data = baseDir + "/data/scripts";
    if (mkdir(scripts_data.c_str(), 0777) == -1)
    {
      perror("Erreur lors de la création du dossier crosstoolsDir");
    }

    std::string sysrootDir = crosstoolsDir + "/sysroot";
    toolchain->sysrootPath = sysrootDir;
    if (mkdir(sysrootDir.c_str(), 0777) == -1)
    {
      perror("Erreur lors de la création du dossier sysrootDir");
    }

    std::string debugrootDir = crosstoolsDir + "/debugroot";
    if (mkdir(debugrootDir.c_str(), 0777) == -1)
    {
      perror("Erreur lors de la création du dossier debugrootDir");
    }

    // other assets

    std::string CMD_AddToGroup = "groupadd vortex";
    std::string CMD_AddUser = "useradd -s /bin/bash -g vortex -m -k /dev/null vortex"; // + " -p " + user.vxHostUser_Crypto;
    std::string CMD_CreateUserDirectory = "mkdir -pv /home/vortex";
    std::string CMD_GiveUserDirectoryToUser = "sudo chown -v vortex:vortex  /home/vortex";
    std::string CMD_AddToSudoers = "sudo usermod -aG root vortex";

    system((char *)CMD_AddToGroup.c_str());
    system((char *)CMD_AddUser.c_str());
    system((char *)CMD_CreateUserDirectory.c_str());
    system((char *)CMD_GiveUserDirectoryToUser.c_str());
    system((char *)CMD_AddToSudoers.c_str());

    // Give toolchain to user
    std::string cmd = "sudo chown -v -R vortex " + baseDir + "/*";
    system((char *)cmd.c_str());
  }

  void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override
  {

    // Get time
    // Get timer
    // Get all...
    this->stop();
    this->result_props = result_properties;
    this->state = finish_state;

    if (this->props)
    {
      std::shared_ptr<VxHost> host = this->props->get<std::shared_ptr<VxHost>>("host", nullptr);
      std::shared_ptr<Task> selfinstance = this->props->get<std::shared_ptr<Task>>("self", nullptr);

      if (host && selfinstance)
      {
        host->currentLoadedSystem.Save(host);
      }
      else
      {
        std::cout << "Error: Host or self instance is null" << std::endl;
      }
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
