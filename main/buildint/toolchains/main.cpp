#include "../../include/vortex.h"
#include "./include/ToolchainInstance.h"

static std::vector<std::shared_ptr<ToolchainInstance>> instances;

class ToolchainsModule : public ModuleInterface
{
public:
    void execute() override
    {
        std::cout << "Plugin One Example executed" << std::endl;

        this->m_logo = icons::_i;        
        this->m_logo_size = icons::_i_size;
    }
    void render() override
    {
        for (auto instance : instances)
        {
            if (instance->render() == "closed")
            {
            };
        }
    }
};

extern "C" ModuleInterface *create_em()
{
    return new ToolchainsModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
