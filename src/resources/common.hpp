#pragma once
#include "helper.hpp"

#include <map>

extern std::string const sFixVersion;
extern std::string sExeName;
extern std::filesystem::path sGameSavePath;

extern HMODULE engineModule;


struct GameInfo
{
    std::string GameTitle;
    std::string ExeName;
    int SteamAppId;
};
extern const GameInfo* game;

enum MgsGame : std::uint8_t
{
    NONE     = 0,
    MGS2     = 1 << 0,
    MGS3     = 1 << 1,
    MG       = 1 << 2,
    LAUNCHER = 1 << 3,
    UNKNOWN  = 1 << 4
};
extern const std::map<MgsGame, GameInfo> kGames;
extern MgsGame eGameType;

extern HMODULE baseModule;
extern std::string sGameVersion;
extern std::filesystem::path sExePath;
extern std::string sFixName;


//Config Options
extern int iOutputResY;
extern int iInternalResY;
extern float fAspectRatio;
extern bool bOutdatedReshade;
extern bool bLauncherConfigSkipLauncher;

