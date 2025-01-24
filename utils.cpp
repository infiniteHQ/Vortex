#define SDL_MAIN_HANDLED

#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include "lib/json/single_include/nlohmann/json.hpp"

std::string getHomeDirectory()
{
#if defined(_WIN32) || defined(_WIN64)
    const char *homePath = std::getenv("USERPROFILE");
    if (homePath == nullptr)
    {
        const char *homeDrive = std::getenv("HOMEDRIVE");
        const char *homePathEnv = std::getenv("HOMEPATH");
        if (homeDrive == nullptr || homePathEnv == nullptr)
        {
            throw std::runtime_error("HOMEPATH environment variables not set");
        }
        return std::string(homeDrive) + std::string(homePathEnv);
    }
    return std::string(homePath);
#else

    const char *homePath = std::getenv("HOME");
    if (homePath == nullptr)
    {
        throw std::runtime_error("HOME environment variable not set");
    }
    return std::string(homePath);
#endif
}

void removeSessionFromJson(const std::string &session_id)
{
    // Set path depending on platform
    std::string json_path;
#if defined(_WIN32) || defined(_WIN64)
    json_path = getHomeDirectory() + "\\.vx\\sessions\\active_sessions.json";
#else
    json_path = getHomeDirectory() + "/.vx/sessions/active_sessions.json";
#endif

    std::ifstream file_in(json_path);
    nlohmann::json active_sessions;

    // Load existing active sessions from the JSON file
    if (file_in.is_open())
    {
        file_in >> active_sessions;
        file_in.close();
    }

    // Find and remove the session with the given session_id
    for (auto it = active_sessions["sessions"].begin(); it != active_sessions["sessions"].end(); ++it)
    {
        if ((*it)["session_id"] == session_id)
        {
            active_sessions["sessions"].erase(it);
            break; // Exit loop after removing the session
        }
    }

    // Save the updated sessions back to the JSON file
    std::ofstream file_out(json_path);
    file_out << active_sessions.dump(4); // Pretty print with indentation
}

/**
 * @brief : Entry point of main Vortex runtime command.
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage : " << std::endl;
    }
    else
    {
        if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
        {
            std::cout << "Usage : " << std::endl;
        }
        else if (std::string(argv[1]) == "-test")
        {
            std::cout << "ok" << std::endl;
            return 0;
        }
        else if (std::string(argv[1]) == "-rms" || std::string(argv[1]) == "--remove_session")
        {
            std::string session_id;
            if (argc > 2)
            {
                std::string arg2 = argv[2];
                if (arg2.rfind("--session_id=\"", 0) == 0 && arg2.back() == '\"')
                {
                    session_id = arg2.substr(13, arg2.length() - 14);
                }
            }

            removeSessionFromJson(session_id);
        }
    }

    return 0;
}