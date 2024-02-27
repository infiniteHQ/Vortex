#include "../../../vortex.h"
struct Task;


struct fullBuildPackage : public Task
{
  void init() override
  {
  };

  void retry() override
  {
  };

  void exec() override;

  void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override
  {

    // Get time 
    // Get timer
    // Get all...

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

TASK(fullBuildPackage);