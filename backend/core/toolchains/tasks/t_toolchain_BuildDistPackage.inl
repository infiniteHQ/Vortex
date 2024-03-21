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

struct BuildDistPackage : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<BuildDistPackage>(*this);
  }

  void init() override
  {
    this->tasktype = "BuildDistPackage";
  };

  // Pour demain :
  /*
    - Finir toutes les tasks de toolchain

  */

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);

    std::shared_ptr<VxPackage> package = this->props->get<std::shared_ptr<VxPackage>>("package", nullptr);

    std::string working_path = std::get<2>(toolchain->currentLoadedSystem.get_varable(this, "dist_path:package_uncompressed:" + package->name + ""));

    this->addIdleCheck("create_build_folder");
    this->addIdleCheck("configure_dist_package");
    this->addIdleCheck("compile_dist_package");
    this->addIdleCheck("install_dist_package");

    /*
        Faire pleins de taches singulières (config, give etc, et faire des plus grosses taches qui controllent et qui call les petites taches)

    */





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
