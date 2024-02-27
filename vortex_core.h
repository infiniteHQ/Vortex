#pragma once

#include "vortex.h"

#include "backend/core/hosts/tasks_host.h"

struct Task;
struct SecondTestHostTask : public Task
{
  void init() override
  {
  };

  void retry() override
  {
  };

  void exec() override
  {
    this->start();
     
  std::this_thread::sleep_for(std::chrono::seconds(3)); // Simuler une tâche en cours d'exécution
     


    std::shared_ptr<hArgs> finishprops = std::make_shared<hArgs>();
    std::string state = "finished";
    this->finish(state, finishprops);
  };

  void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override
  {

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

TASK(SecondTestHostTask);
