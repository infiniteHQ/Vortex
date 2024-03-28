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

struct T_T_CheckCompiler : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<T_T_CheckCompiler>(*this);
  }

  void init() override
  {
    this->tasktype = "T_T_CheckCompiler";

    // Props used by task execution
    this->neededProps.push_back("toolchain");

    // Variables needed by task execution
    //this->neededVariables.push_back("dist_path:package:\[package_name\]");

    // Checklist

    this->addIdleCheck("create_temp_env");
    this->addIdleCheck("create_c_file");
    this->addIdleCheck("populate_c_file");
    this->addIdleCheck("compile_c_file");
    this->addIdleCheck("test_output");
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

    std::string tempPath = ctx->projectPath; 
    tempPath += "/.vx/temp/" + toolchain->name;

    {
      std::string cmd = "mkdir " + tempPath;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("create_temp_env", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("create_temp_env", "failed", output, cmd);
    }


    {
      std::string cmd = "touch " + tempPath + "/main.c";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("create_c_file", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("create_c_file", "failed", output, cmd);
    }

    {
      std::string cmd = "sudo echo \"int main(){return42;}\" >> " + tempPath + "/main.c";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("populate_c_file", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("populate_c_file", "failed", output, cmd);
    }

    {
      std::string cmd = "sudo echo \"#!/bin/sh \n \\$CC main.c -o " + tempPath + "/main \n echo \"Le compilateur utilisé est : $CC\"\" >> " + tempPath + "/main.sh";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("populate_c_file", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("populate_c_file", "failed", output, cmd);
    }

    {
      std::string cmd = "sudo chown -R vortex:vortex " + tempPath;
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("populate_c_file", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("populate_c_file", "failed", output, cmd);
    }

    // CC est egal a " " dans le script généré ! Fix ça, puis finir avec d'autres tests, build et test auto, puis passer a la suite.


    {
      std::string cmd = "sudo -u vortex -i sh -c 'cd " + tempPath + " && bash main.sh";
      auto [output, result] = toolchain->exec_cmd_quote(cmd.c_str());

      if (result == 0)
        this->addCheckVerdict("compile_c_file", "success", output, cmd);
      if (result != 0)
        this->addCheckVerdict("compile_c_file", "failed", output, cmd);
    }


    {
      std::string cmd = "sudo ./" + tempPath + "/main";
      auto [output, result] = toolchain->exec_cmd(cmd.c_str());

      if (result == 42)
        this->addCheckVerdict("test_output", "success", output, cmd);
      if (result != 42)
        this->addCheckVerdict("test_output", "failed", output, cmd);
    }


      std::string cmd = "sudo echo \"CC=" + toolchain->distToolchain.CC + "\" >> /home/vortex/.bashrc";


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
