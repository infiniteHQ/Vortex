#include "../../../include/vortex.h"

struct TaskModule
{
    virtual std::shared_ptr<TaskModule> clone() const
    {
        return std::make_shared<TaskModule>(*this);
    }

    std::string id; // to find this task from everywhere
    std::string tasktype = "unknow";

    std::vector<std::string> neededVariables; // Only data view

    std::vector<std::string> neededProps;                       // Only data view
    std::vector<std::pair<std::string, std::string>> env_props; // To resolve and store in " std::shared_ptr<hArgs> props"

    std::string component;
    std::string state; // state of this task
    std::vector<std::tuple<std::string, std::string, std::string>> used_variables;
    std::vector<std::tuple<std::string, std::string, std::string>> created_variables;

    int priority;

    int warningCounter = 0;
    int failCounter = 0;
    int unknowCounter = 0;
    int successCounter = 0;

    std::vector<std::shared_ptr<Check>> checkList;
    void addIdleCheck(std::string id)
    {
        std::shared_ptr<Check> newCheck = std::make_shared<Check>();
        newCheck->checkID = id;
        this->checkList.push_back(newCheck);
        this->unknowCounter++;
    }

    void updateState()
    {
        std::string _state = "unknow";
        if (this->successCounter > 0)
        {
            _state = "success";
        }
        else if (this->warningCounter > 0)
        {
            _state = "warning";
        }
        else if (this->failCounter > 0)
        {
            _state = "failed";
        }
        this->state = _state;
    }

    /*
        Result types :
        - success : The check is a success
        - warning : The check is a warning
        - failed : The check is a failure
        - unknow : The check is not yet processed
        - fatal : The check is a fatal error
    */

    void addCheckVerdict(std::string id, std::string result, std::string log, std::string directive)
    {
        for (auto check : this->checkList)
        {
            if (check->checkID == id)
            {
                check->checkLog = log;
                check->checkResult = result;
                check->checkDirective = directive;

                if (check->checkResult == "failed")
                {
                    this->failCounter++;
                    this->unknowCounter--;
                }
                else if (check->checkResult == "success")
                {
                    this->successCounter++;
                    this->unknowCounter--;
                }
                else if (check->checkResult == "warning")
                {
                    this->warningCounter++;
                    this->unknowCounter--;
                }
                else if (check->checkResult == "unknow")
                {
                }
                this->updateState();
            }
        }
    }

    // type // state
    std::vector<std::pair<std::string, std::string>> depsChecks;
    // type // name
    std::vector<std::pair<std::string, std::string>> depsChecksSpec;

    bool ifProps(const std::vector<std::string> &propsname)
    {
        int satisfied = 0;
        int total = propsname.size();

        this->depsChecks.clear();

        for (const auto &prop : propsname)
        {
            bool propFound = false;

            std::string propStr = prop;

            for (const auto &registeredArg : this->props->registered_arguments)
            {
                std::string registeredArgStr = registeredArg.c_str();

                if (registeredArgStr == propStr)
                {
                    satisfied++;
                    this->depsChecks.push_back({propStr, "satisfied"});
                    VortexMaker::LogInfo("Core", "New prop found, state: " + std::to_string(satisfied) + "/" + std::to_string(total) + " props found.");
                    propFound = true;
                    break;
                }
            }

            if (!propFound)
            {
                this->depsChecks.push_back({propStr, "missing"});
            }
        }

        VortexMaker::LogInfo("Core", "Added " + std::to_string(total) + " props to check.");

        // Vérifier si toutes les propriétés sont satisfaites
        return (satisfied == total);
    }

    bool ifProp(std::string propname)
    {
        for (auto prop : this->props->registered_arguments)
        {
            if (prop.c_str() == propname)
            {
                return true;
            }
        }
        return false;
    }

    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    double m_TotalTime;
    bool m_bRunning = false;

    double elapsedMilliseconds()
    {
        std::chrono::time_point<std::chrono::system_clock> endTime;
        if (m_bRunning)
        {
            endTime = std::chrono::system_clock::now();
        }
        else
        {
            endTime = m_EndTime;
        }
        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
    }
    double elapsedSeconds() { return elapsedMilliseconds() / 1000.0; }

    std::string startTime()
    {
        std::time_t t = std::chrono::system_clock::to_time_t(this->m_StartTime);
        std::string ts = std::ctime(&t);
        ts.resize(ts.size() - 1);
        return ts;
    }

    void start()
    {
        this->m_StartTime = std::chrono::system_clock::now();
        this->state = "process";
        this->m_bRunning = true;
    }

    void stop()
    {
        this->m_EndTime = std::chrono::system_clock::now();
        this->m_bRunning = false;
        this->m_TotalTime = elapsedSeconds();
    }

    // Custom args
    std::shared_ptr<hArgs> props;

    // Result
    std::shared_ptr<hArgs> result_props;

    // std::shared_ptr<VxHost> parent;

    virtual void init() { VortexMaker::LogError("Core", "Core task not implemented !"); };
    virtual void retry() { VortexMaker::LogError("Core", "Core task not implemented !"); };
    virtual void exec() { VortexMaker::LogError("Core", "Core task not implemented !"); };
    virtual void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) { VortexMaker::LogError("Core", "Core task not implemented !"); };
};