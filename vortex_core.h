#pragma once

#include "vortex.h"

struct Task;

struct TestHostTask : public Task{
    void exec() override {      
      std::cout << "Exec test tasks yey";
      

      std::shared_ptr<hArgs> finishprops = std::make_shared<hArgs>();
      
      std::string state = "finished";
      this->finish(state, finishprops);
    };

    void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override {
      this->result_props = result_properties;
      this->state = finish_state;

      std::shared_ptr<VxHost> host = this->props->get<std::shared_ptr<VxHost>>("host", nullptr);      
      std::shared_ptr<TestHostTask> selfinstance = this->props->get<std::shared_ptr<TestHostTask>>("self", nullptr);
      
      host->currentLoadedSystem.executedTasks.push_back(selfinstance);
      //host->currentLoadedSystem.Extract();
    };
};