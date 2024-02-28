#include "../../../vortex.h"
struct Task;

struct deployPackageToDist : public Task
{
  void init() override{};

  void retry() override{};

  void exec() override;

  void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override
  {

    // Get time
    // Get timer
    // Get all...

    finish_state = "unknow";

    if (this->failCounter > 0)
    {
      finish_state = "failed";
    }
    else
    {
      if (this->warningCounter > 0)
      {
        finish_state = "warning";
      }
      else
      {
        if (this->successCounter > 0)
        {
          finish_state = "success";
        }
      }
    }

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

struct configurePackage : public Task
{
  void init() override{};

  void retry() override{};

  void exec() override;

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

struct fullBuildPackage : public Task
{
  void init() override{};

  void retry() override{};

  void exec() override;

  void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override
  {

      finish_state = "unknow";
    if(this->successCounter > 0){
      finish_state = "success";
    }
    
    if(this->warningCounter > 0){
      finish_state = "warning";
    }

    if(this->failCounter > 0){
      finish_state = "failed";
    }


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

TASK(fullBuildPackage);