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

struct CreateTemporaryUser : public Task
{
  void init() override
  {
    this->tasktype = "CreateTemporaryUser";
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    VxContext *ctx = VortexMaker::GetCurrentContext();


    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);




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

    std::string bashprofile = "cat > ~/.bash_profile << \"EOF\" \n exec env -i HOME=$HOME TERM=$TERM PS1='\\u:\\w\\$ ' /bin/bash \n EOF";

    std::string bashrc = "cat > ~/.bashrc << \"EOF\" set +h \n umask 022 \n LC_ALL=POSIX \n PATH=/usr/bin \n if [ ! -L /bin ]; then PATH=/bin:" + toolchain->crosstoolsPath + "; fi \n PATH=" + toolchain->crosstoolsPath + "/tools/bin:$PATH \n CONFIG_SITE=" + toolchain->crosstoolsPath + "/usr/share/config.site \n export LFS LC_ALL LFS_TGT PATH CONFIG_SITE \n EOF";

    std::string save_bashprofile = "[ ! -e /etc/bash.bashrc ] || mv -v /etc/bash.bashrc /etc/bash.bashrc.NOUSE";
    std::string reload_bashprofile = "source ~/.bash_profile";

    std::string as_bashprofile = "su - vortex -c \"" + bashprofile + "\"";
    std::string as_bashrc = "su - vortex -c \"" + bashrc + "\"";
    std::string as_save_bashprofile = "su - vortex -c \"" + save_bashprofile + "\"";
    std::string as_reload_bashprofile = "su - vortex -c \"" + reload_bashprofile + "\"";

    system("touch /home/vortex/test.txt");

    for (auto package : toolchain->packages)
    {
      package->AddDiag("decompression");
      package->AddDiag("configuration");
      package->AddDiag("compilation");
      package->AddDiag("installation");

      for (auto action : package->actions)
      {
        if (action->type == "command")
        {
          std::string label = "action-" + action->type + "-" + action->executionSequence + "-";
          label += action->priority;

          package->AddDiag(label);
        }
      }

      std::string cmd = "cp -r " + package->path + " " + toolchain->packages_data;

      size_t posDernierSlash = package->path.find_last_of('/');
      if (posDernierSlash != std::string::npos)
      {
        std::string resultat = package->path.substr(posDernierSlash + 1);
        package->distPath = toolchain->packages_data + "/" + resultat;
      }

      int returnCode = system(cmd.c_str());
      package->SetDiagCode("Prepare", returnCode);

      if (returnCode == 0)
      {
        // package->SetDiagStatus("Prepare", "Success");
      }
      else
      {
        // package->SetDiagStatus("Prepare", "Fail");
      }
    }

    std::string cuser = "chown vortex:vortex -R " + toolchain->envPath + "/*";
    system(cuser.c_str());
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
