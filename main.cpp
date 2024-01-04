#include <iostream>
#include <string>
#include <thread>

#include "./tools/devtools/app/src/devtools.h"
#include "./vortex.h"

int main(int argc, char *argv[])
{

    VxContext* ctx = VortexMaker::CreateContext();

    if (argc < 2)
    {
        std::cout << std::endl;
        std::cout << "Usage : vortex <paramater(s)...> <information(s)...>" << std::endl << std::endl;
        std::cout << "-h --help :               See all parameters" << std::endl;
        std::cout << "-g --gui :                Open the Vortex graphical interface" << std::endl;
        std::cout << "-I --init  <...> :        Initialize a new project" << std::endl;
        std::cout << "-i --install  <...> :     Install a new content" << std::endl;
        std::cout << "-b --build <...> :        Build the project" << std::endl;
        std::cout << "-a --add <...> :          Add a content (packages, tools, libs, toolchains...)" << std::endl;
        std::cout << "-p --preview <...> :      Preview final component(s) of the project" << std::endl << std::endl;
        std::cout << "Vortex makes it easy to create a system, application or toolchain. It also offers a wide range of options for system maintenance and supervision. " << std::endl << std::endl;
    }
    else
    {
        if (std::string(argv[1]) == "-g" || std::string(argv[1]) == "--gui")
        {
            std::cout << "Opening the graphical interface..." << std::endl;

            std::thread receiveThread;
            std::thread Thread([&]()
                               { VortexMaker::DebugTools(argc, argv, ctx); });
            receiveThread.swap(Thread);
            receiveThread.join();
        }
        else if (std::string(argv[1]) == "-b" || std::string(argv[1]) == "--build")
        {
            if (argc < 3)
            {
                std::cout << "Usage : vortex -b <build_type>" << std::endl;
                std::cout << "Or : vortex --build <build_type>" << std::endl;
                std::cout << "<build_type> : " << std::endl;
                std::cout << "all :             Build the entire project" << std::endl;
                std::cout << "toolchains :      Build all toochains" << std::endl;
                std::cout << "toolchain <name>: Build specific toolchain" << std::endl;
                std::cout << "packages :        Build all packages" << std::endl;
                std::cout << "package <name> :  Build specific package" << std::endl;
            }
            else
            {
                if (std::string(argv[2]) == "all")
                {
                }
                else if (std::string(argv[2]) == "toolchains")
                {
                }
                else if (std::string(argv[2]) == "toolchain")
                {
                    if (argc < 4)
                    {
                        std::cout << "Please enter the toolchain path" << std::endl;
                        std::cout << "Usage : vortex -b toolchain <path>" << std::endl;
                    }
                }
                else if (std::string(argv[2]) == "packages")
                {
                }
                else if (std::string(argv[2]) == "package")
                {
                    if (argc < 4)
                    {
                        std::cout << "Please enter the package path" << std::endl;
                        std::cout << "Usage : vortex -b package <path>" << std::endl;
                    }
                }
            }
        }
        else if (std::string(argv[1]) == "-a" || std::string(argv[1]) == "--add")
        {
            if (argc < 3)
            {
                std::cout << "Usage : vortex -a <content_type>" << std::endl;
                std::cout << "Or : vortex --add <content_type>" << std::endl;
                std::cout << "<content_type> : " << std::endl;
                std::cout << "package <path>:   Build the entire project" << std::endl;
                std::cout << "lib <path>:       Build all toochains" << std::endl;
                std::cout << "toolchain <path>: Build specific toolchain" << std::endl;
            }
        }
        else if (std::string(argv[1]) == "-p" || std::string(argv[1]) == "--preview")
        {
            if (argc < 3)
            {
            }
        }
    }

    return 0;
}