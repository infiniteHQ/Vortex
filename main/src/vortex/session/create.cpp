#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void VortexMaker::CreateSessionTopic(const std::string &post_topic)
{
#if defined(_WIN32) || defined(_WIN64)
    std::string topic_path = VortexMaker::getHomeDirectory() + "\\.vx\\sessions\\" + post_topic;
#else
    std::string topic_path = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + post_topic;
#endif

    try
    {
        if (fs::create_directories(topic_path))
        {
            VortexMaker::LogInfo("Core", "Session \"" + post_topic + "\" is created with success!");
        }
        else
        {
            VortexMaker::LogError("Core", "Session \"" + post_topic + "\" already exists or could not be created!");
        }
    }
    catch (const std::exception &e)
    {
        VortexMaker::LogError("Core", "Failed to create session \"" + post_topic + "\": " + e.what());
    }

#if defined(_WIN32) || defined(_WIN64)
    std::string crash_path = topic_path + "\\crash";
#else
    std::string crash_path = topic_path + "/crash";
#endif

    try
    {
        if (fs::create_directories(crash_path))
        {
            VortexMaker::LogInfo("Core", "Session crash folder of \"" + post_topic + "\" is created with success!");
        }
        else
        {
            VortexMaker::LogError("Core", "Session crash folder of \"" + post_topic + "\" already exists or could not be created!");
        }
    }
    catch (const std::exception &e)
    {
        VortexMaker::LogError("Core", "Failed to create session crash folder of \"" + post_topic + "\": " + e.what());
    }

#if defined(_WIN32) || defined(_WIN64)
    std::string log_path = topic_path + "\\logs";
#else
    std::string log_path = topic_path + "/logs";
#endif

    try
    {
        if (fs::create_directories(log_path))
        {
            VortexMaker::LogInfo("Core", "Session logs folder of \"" + post_topic + "\" is created with success!");
        }
        else
        {
            VortexMaker::LogError("Core", "Session logs folder of \"" + post_topic + "\" already exists or could not be created!");
        }
    }
    catch (const std::exception &e)
    {
        VortexMaker::LogError("Core", "Failed to create session logs folder of \"" + post_topic + "\": " + e.what());
    }

    {
#if defined(_WIN32) || defined(_WIN64)
        std::string file_path = crash_path + "\\core_dump.txt";
#else
        std::string file_path = crash_path + "/core_dump.txt";
#endif

        std::ofstream file(file_path);
        if (file.is_open())
        {
            file.close();
            VortexMaker::LogInfo("Core", "Session crash folder of \"" + post_topic + "\" is created with success!");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create session crash folder of \"" + post_topic + "\"!");
        }
    }

    {
#if defined(_WIN32) || defined(_WIN64)
        std::string file_path = crash_path + "\\log_dump.txt";
#else
        std::string file_path = crash_path + "/log_dump.txt";
#endif

        std::ofstream file(file_path);
        if (file.is_open())
        {
            file.close();
            VortexMaker::LogInfo("Core", "Session crash folder of \"" + post_topic + "\" is created with success!");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create session crash folder of \"" + post_topic + "\"!");
        }
    }

    {
#if defined(_WIN32) || defined(_WIN64)
        std::string file_path = topic_path + "\\vortex.json";
#else
        std::string file_path = topic_path + "/vortex.json";
#endif

        std::ofstream file(file_path);
        if (file.is_open())
        {
            file.close();
            VortexMaker::LogInfo("Core", "Session topic files of \"" + post_topic + "\" is created with success!");
        }
        else
        {
            VortexMaker::LogError("Core", "Failed to create topic files of \"" + post_topic + "\"!");
        }
    }
}