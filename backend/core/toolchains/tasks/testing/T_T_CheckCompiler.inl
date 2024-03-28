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

struct T_P_ConfigurePackage : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<T_P_ConfigurePackage>(*this);
  }

  void init() override
  {
    this->tasktype = "T_P_ConfigurePackage";

    // Props used by task execution
    this->neededProps.push_back("toolchain");

    // Variables needed by task execution
    //this->neededVariables.push_back("dist_path:package:\[package_name\]");

    // Checklist

    this->addIdleCheck("load_dist_toolchain");
    this->addIdleCheck("test_links");
    this->addIdleCheck("test_ld");
    this->addIdleCheck("test_compiler");
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    if(!this->ifProps(this->neededProps)){this->finish("fatal", nullptr);}

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
    this->depsChecksSpec.push_back({"toolchain",toolchain->name});

    // Load dist toolchain (bash rc, etc...)



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
      std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
      toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
