//
//  main.cpp
//  Entry point for Vortex editor and crash handler
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#define SDL_MAIN_HANDLED

#include <fstream>
#include <iostream>
#include <string>

// UI instances
#include "./lib/cherry/cherry.hpp"
#include "./vxcore/include/templates/load.hpp"
#include "./vxcore/include/vortex.h"
#include "./vxgui/crash_handler/crash_handler.hpp"
#include "./vxgui/editor/main/editor.hpp"

static std::string session_id = "unknow";

void PrintInfinite() {
  std::cout << R"(
                  ███  ███   ███  ███████▓▒███ ███░  ░███ ███▓█████████ ████████                   
                  ███  ███▒  ███  ███████▓▒███ █████ ░███ ███▓█████████ ████████                   
                  ███  ████  ███  ███▓▓▓▓ ▒███ ██████▒███ ███▓   ███░   ███                        
                  ███  █████ ███  ███████ ▒███ ███▒██████ ███▓   ███░   ███                        
                  ███  █████████  ███     ▒███ ███▒ █████ ███▓   ███░   ███                        
                  ███  ███▓█████  ███     ▒███ ███▒  ████ ███▓   ███░   ███                        
                  ███  ███ █████  ███                     ███▓   ███░   ███████                    
                  ███  ███ ▒████  ██░                        ░   ███░   ███████                    
                  ███  ███  ████                                 ███░   ███                        
                  ███  ███  ░█▓                                    █░   ███                        
                  ███  ███                                              ███                        
                  ███  ███                                              ███▒                       
                  ███  ██                                               ░████                      
                  ███                                                     ████▒                    
                  ███                                                       ███░                   
                  ██▒                                                        ███                   
                  █░                                                          ██                   
                  █                                                            █                   
                                                                                                              )"
            << std::endl;
}

void print_header(const std::string &additions = "") {
  // Print this every time
  std::cout << std::endl;
  std::cout << "\033[38;2;177;255;49m";
  PrintInfinite();
  std::cout << "\033[0m";

  std::cout << "\033[38;2;177;255;49m"
            << " Vortex " << VORTEX_VERSION << " " << additions << "\033[0m" << std::endl;

  std::cout << "┌──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┐"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Description : "
            << "\033[0m"
            << "                                                               "
               "                  │ "
            << std::endl;

  std::cout << "│ ❓ Vortex is a complete open creation platform that contain "
               "a bunch of tools for creators and  │ "
            << std::endl;
  std::cout << "│  makers.                                                     "
               "                                  │ "
            << std::endl;
  std::cout << "├──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┤"
            << std::endl;

  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Usage : "
            << "\033[0m"
            << "vortex <paramater(s)...> <information(s)...>                   "
               "                        │ "
            << std::endl;

  std::cout << "├──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┤"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Helpfull commands : "
            << "\033[0m"
            << "                                                               "
               "            │ "
            << std::endl;
  std::cout << "│ -h   --help :                          See all parameters    "
               "                                  │ "
            << std::endl;
  std::cout << "│ -l   --launcher :                      Open the Vortex "
               "launcher (for projects, components...)  │ "
            << std::endl;
  std::cout << "│ -e   --editor :                        Open the Vortex "
               "graphical interface                     │ "
            << std::endl;
  std::cout << "│ -cp  --create-project  <...> :         Create a new project  "
               "                                  │ "
            << std::endl;
  std::cout << "│ -i   --install  <...> :                Install a ressource "
               "into the user environment           │ "
            << std::endl;
  std::cout << "│ -bi  --build-install  <...> :          Build & Install a "
               "ressource into the user environment   │ "
            << std::endl;
  std::cout << "├──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┤"
            << std::endl;
  std::cout << "│"
            << "\033[38;2;177;255;49m"
            << " Links : "
            << "\033[0m"
            << "                                                               "
               "                        │ "
            << std::endl;
  std::cout << "│ 🌍 Vortex website : https://infinite.si/                     "
               "                                  │ "
            << std::endl;
  std::cout << "│ 📝 News : https://infinite.si/blog                           "
               "                                  │ "
            << std::endl;
  std::cout << "│ 💬 Discord : https://discord.gg/H2wptkecUg                   "
               "                                  │ "
            << std::endl;
  std::cout << "│ 🧑‍💻 GitHub : https://github.com/infiniteHQ/Vortex        "
               "                                       │ "
            << std::endl;
  std::cout << "└──────────────────────────────────────────────────────────────"
               "──────────────────────────────────┘ "
            << std::endl;
  std::cout << std::endl;
}

bool CheckDirectory() {
  std::ifstream mainconfig("vortex.config");
  if (!mainconfig.good()) {
    std::cout << "This directory does not contain a Vortex project. Please "
                 "initialize a new project with \"vortex --create-project "
                 "<project_name>\" or repair it."
              << std::endl;
    return false;
  }
  return true;
}

std::shared_ptr<VxContext> init_runtime() {
  auto ctx = vxe::create_context();

  vxe::initialize_platform_vendor();
  vxe::add_credits("vx", Cherry::GetPath("CREDITS"));

  ctx->state.session_id = session_id;

  vxe::create_global_logger();
  vxe::create_console_logger();
  ctx->logger = true;

  vxe::create_session_topic(ctx->state.session_id);

  // initialize environment
  vxe::init_environment();
  vxe::detect_platform();
  vxe::detect_arch();

  // Refresh environment registered projects
  vxe::refresh_environment_projects();

  vxe::load_system_templates(ctx->IO.sys_templates);

  std::ifstream file("vortex.config");

  if (file) {
    nlohmann::json jsonContent;
    file >> jsonContent;
    vxe::init_project(jsonContent);
  }

  return ctx;
}

std::shared_ptr<VxContext> init_blank_runtime() {
  auto ctx = vxe::create_context();

  ctx->state.session_id = session_id;

  // Link credits file
  vxe::add_credits("vx", Cherry::GetPath("CREDITS"));
  vxe::initialize_platform_vendor();
  vxe::create_global_logger();
  vxe::create_console_logger();
  vxe::log_info("Bootstrapp", "Initializing runtime...");

  vxe::create_session_topic(ctx->state.session_id);

  // initialize environment
  vxe::init_environment();
  vxe::detect_platform();
  vxe::detect_arch();
  vxe::refresh_environment_projects();
  vxe::load_system_templates(ctx->IO.sys_templates);

  ctx->logger = true;
  return ctx;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    print_header();
  } else {
    if (std::string(argv[1]) == "-test") {
      return 0;
    } else if (std::string(argv[1]) == "--crash" || std::string(argv[1]) == "--get-last-crash") {
      print_header("(Crash handler)");

      if (argc > 2) {
        std::string arg2 = argv[2];
        if (arg2.rfind("--session_id=", 0) == 0) {
          session_id = arg2.substr(13);
          if (!session_id.empty() && session_id.front() == '"' && session_id.back() == '"') {
            session_id = session_id.substr(1, session_id.size() - 2);
            g_SessionID = session_id;
          }
        }
      }

      init_blank_runtime();
      vxe::log_info("Bootstrapp", "Opening the graphical interface...");
      vxe::VortexCrash(argc, argv);
    } else if (std::string(argv[1]) == "-e" || std::string(argv[1]) == "--editor") {
      print_header("(Editor)");
      if (argc > 2) {
        std::string arg2 = argv[2];
        if (arg2.rfind("--session_id=", 0) == 0) {
          session_id = arg2.substr(13);
          if (!session_id.empty() && session_id.front() == '"' && session_id.back() == '"') {
            session_id = session_id.substr(1, session_id.size() - 2);
            g_SessionID = session_id;
          }
        }
      }

      auto ctx = init_runtime();
      vxe::log_info("Bootstrapp", "Opening the graphical interface...");

      vxe::VortexEditor(argc, argv);
    }
  }

  return 0;
}