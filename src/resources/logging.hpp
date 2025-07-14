#pragma once
#include <spdlog/spdlog.h>

class Logging final
{
private: 
    static std::string GetSteamOSVersion();
public:
    static void Initialize();
    static void LogSysInfo();
    std::chrono::time_point<std::chrono::high_resolution_clock> initStartTime;
    bool bIsSteamDeck = false;
    bool bCheckedSteamDeck = false;
};

inline Logging g_Logging;


inline std::string lastLoaded;

#define INITIALIZE(func) \
    do { \
        std::chrono::time_point<std::chrono::high_resolution_clock> currentInitPhaseStartTime;\
        if(strcmp(#func,"InitializeSubsystems()") == 0) \
        {\
            spdlog::info("---------- Subsystem initialization started ----------", #func); \
            currentInitPhaseStartTime = g_Logging.initStartTime;\
        }\
        else if(!lastLoaded.empty())\
        {\
            spdlog::info("---------- {}\tNow loading: {} ----------", lastLoaded, #func); \
            currentInitPhaseStartTime = std::chrono::high_resolution_clock::now();\
        }\
        else\
        {\
            spdlog::info("---------- Loading: {} ----------", #func); \
            currentInitPhaseStartTime = std::chrono::high_resolution_clock::now();\
        }\
        (func); \
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - currentInitPhaseStartTime).count();\
        if(strcmp(#func,"InitializeSubsystems()") == 0) \
        {\
            if(!lastLoaded.empty())\
            {\
                spdlog::info("---------- {} ----------", lastLoaded); \
            }\
            spdlog::info("---------- All intial systems completed loading in: {} ms. ----------", duration); \
        }\
        else\
        {\
            lastLoaded = std::string(#func) + " loaded in: " + std::to_string(duration) + " ms."; \
        }\
    } while (0)
