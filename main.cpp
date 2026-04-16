#define SDL_MAIN_HANDLED

#include <fstream>
#include <iostream>
#include <string>

// UI instances
#include "./vxgui/crash_handler/crash_handler.hpp"
#include "./vxgui/editor/app/src/editor.hpp"

#include "./lib/cherry/cherry.hpp"
#include "./vxcore/include/templates/load.h"
#include "./vxcore/include/vortex.h"

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

void PrintHeader(const std::string &additions = "") {
  // Print this every time
  std::cout << std::endl;
  std::cout << "\033[38;2;177;255;49m";
  PrintInfinite();
  std::cout << "\033[0m";

  std::cout << "\033[38;2;177;255;49m"
            << " Vortex " << VORTEX_VERSION << " " << additions << "\033[0m"
            << std::endl;

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
  std::cout
      << "│ 🧑‍💻 GitHub : https://github.com/infiniteHQ/Vortex        "
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

VxContext *InitRuntime(bool logger) {
  VxContext *ctx = VortexMaker::CreateContext();

  VortexMaker::InitializePlatformVendor();
  VortexMaker::AddCredits("vx", Cherry::GetPath("CREDITS"));

  ctx->state.session_id = session_id;

  VortexMaker::CreateGlobalLogger();
  VortexMaker::CreateConsoleLogger();
  ctx->logger = logger;

  VortexMaker::CreateSessionTopic(ctx->state.session_id);

  // Initialize environment
  VortexMaker::InitEnvironment();
  VortexMaker::DetectPlatform();
  VortexMaker::DetectArch();

  // Refresh environment registered projects
  VortexMaker::RefreshEnvironmentProjects();

  VortexMaker::LoadSystemTemplates(ctx->IO.sys_templates);

  std::ifstream file("vortex.config");

  if (file) {
    nlohmann::json jsonContent;
    file >> jsonContent;
    VortexMaker::InitProject(jsonContent);
  }

  return ctx;
}
// Project sys size== 0

// Project creator,
// Template deployment overrides (project, modules_content, etc...)

VxContext *InitBlankRuntime(bool logger) {
  VxContext *ctx = VortexMaker::CreateContext();

  ctx->state.session_id = session_id;

  // Link credits file
  VortexMaker::AddCredits("vx", Cherry::GetPath("CREDITS"));
  VortexMaker::InitializePlatformVendor();
  VortexMaker::CreateGlobalLogger();
  VortexMaker::CreateConsoleLogger();
  VortexMaker::LogInfo("Bootstrapp", "Initializing runtime...");

  VortexMaker::CreateSessionTopic(ctx->state.session_id);

  // Initialize environment
  VortexMaker::InitEnvironment();
  VortexMaker::DetectPlatform();
  VortexMaker::DetectArch();

  // Refresh environment registered projects
  VortexMaker::RefreshEnvironmentProjects();

  VortexMaker::LoadSystemTemplates(ctx->IO.sys_templates);

  ctx->logger = logger;
  return ctx;
}

/**
 * @brief : Entry point of main Vortex runtime command.
 */
int main(int argc, char *argv[]) {
  if (argc < 2) {
    PrintHeader();
  } else {
    if (std::string(argv[1]) == "-test") {
      return 0;
    } else if (std::string(argv[1]) == "--crash" ||
               std::string(argv[1]) == "--get-last-crash") {
      PrintHeader("(Crash handler)");

      if (argc > 2) {
        std::string arg2 = argv[2];
        if (arg2.rfind("--session_id=", 0) == 0) {
          session_id = arg2.substr(13);
          if (!session_id.empty() && session_id.front() == '"' &&
              session_id.back() == '"') {
            session_id = session_id.substr(1, session_id.size() - 2);
            g_SessionID = session_id;
          }
        }
      }

      InitBlankRuntime(true);
      VortexMaker::LogInfo("Bootstrapp", "Opening the graphical interface...");
      VortexMaker::VortexCrash(argc, argv);
    } else if (std::string(argv[1]) == "-e" ||
               std::string(argv[1]) == "--editor") {
      PrintHeader("(Editor)");
      if (argc > 2) {
   std::string arg2 = argv[2];
        if (arg2.rfind("--session_id=", 0) == 0) {
          session_id = arg2.substr(13);
          if (!session_id.empty() && session_id.front() == '"' &&
              session_id.back() == '"') {
            session_id = session_id.substr(1, session_id.size() - 2);
            g_SessionID = session_id;
          }
        }
      }

      InitRuntime(true);
      VortexMaker::LogInfo("Bootstrapp", "Opening the graphical interface...");

      VortexMaker::VortexEditor(argc, argv);
    }
  }

  return 0;
}