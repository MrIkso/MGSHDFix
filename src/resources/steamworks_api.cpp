#include "common.hpp"
#include "steamworks_api.hpp"

#include <logging.hpp>

/* Example usage of the SteamAPI class to set an achievement.
 if (g_SteamAPI.SetAchievement("ACH_WIN_ONE_GAME"))
{
    g_SteamAPI.StoreStats();
    spdlog::info("Achievement unlocked!");
}
else
{
    spdlog::error("Failed to unlock achievement.");
}
*/

void SteamAPI::Setup()
{
    if (!bIsLegitCopy)
    {
        spdlog::warn("Steam achievement/stat tracking fixes are disabled due to non-legitimate copy.");
        return;
    }
    hSteamAPI = GetModuleHandleA("steam_api64.dll");
    if (!hSteamAPI)
    {
        hSteamAPI = LoadLibraryA("steam_api64.dll");
    }
    if (!hSteamAPI)
    {
        spdlog::error("steam_api64.dll not loaded.");
        return;
    }

    SteamUserFn = reinterpret_cast<SteamAPI_SteamUser_v021_t>(GetProcAddress(hSteamAPI, "SteamAPI_SteamUser_v021"));
    GetSteamIDFn = reinterpret_cast<SteamAPI_ISteamUser_GetSteamID_t>(GetProcAddress(hSteamAPI, "SteamAPI_ISteamUser_GetSteamID"));

    SteamUserStatsFn = reinterpret_cast<SteamAPI_SteamUserStats_v012_t>(GetProcAddress(hSteamAPI, "SteamAPI_SteamUserStats_v012"));
    RequestCurrentStatsFn = reinterpret_cast<SteamAPI_ISteamUserStats_RequestCurrentStats_t>(GetProcAddress(hSteamAPI, "SteamAPI_ISteamUserStats_RequestCurrentStats"));
    SetAchievementFn = reinterpret_cast<SteamAPI_ISteamUserStats_SetAchievement_t>(GetProcAddress(hSteamAPI, "SteamAPI_ISteamUserStats_SetAchievement"));
    ClearAchievementFn = reinterpret_cast<SteamAPI_ISteamUserStats_ClearAchievement_t>(GetProcAddress(hSteamAPI, "SteamAPI_ISteamUserStats_ClearAchievement"));
    StoreStatsFn = reinterpret_cast<SteamAPI_ISteamUserStats_StoreStats_t>(GetProcAddress(hSteamAPI, "SteamAPI_ISteamUserStats_StoreStats"));
    GetAchievementFn = reinterpret_cast<SteamAPI_ISteamUserStats_GetAchievement_t>(GetProcAddress(hSteamAPI, "SteamAPI_ISteamUserStats_GetAchievement"));

    if (!SteamUserFn || !GetSteamIDFn || !SteamUserStatsFn || !RequestCurrentStatsFn || !SetAchievementFn
        || !ClearAchievementFn || !StoreStatsFn || !GetAchievementFn)
    {
        spdlog::error("Failed to load one or more Steam API functions.");
        return;

    if (uint8_t* AfterSteamSetupResult = Memory::PatternScan(baseModule, "48 8B 05 ?? ?? ?? ?? 8B CB", "Steam API Initialization"))
    {
        static SafetyHookMid SteamMidhook {};
        SteamMidhook = safetyhook::create_mid(AfterSteamSetupResult,
            [](SafetyHookContext& ctx)
            {
                g_SteamAPI.FetchAndCacheSteamID();
            });
        spdlog::info("SteamAPI: Hook placed at Steam initialization, functions resolved.");
    }
}

void SteamAPI::FetchAndCacheSteamID()
{
    pSteamUser = SteamUserFn();
    if (!pSteamUser)
    {
        spdlog::error("SteamUser interface not initialized yet.");
        return;
    }

    steamID = GetSteamIDFn(pSteamUser);
    spdlog::info("----------");
    spdlog::info("SteamAPI: SteamID: {}", *steamID);
    spdlog::info("----------");

    pSteamUserStats = SteamUserStatsFn();
    if (!pSteamUserStats)
    {
        spdlog::error("SteamUserStats interface not initialized.");
        return;
    }

    if (!RequestCurrentStats())
    {
        spdlog::warn("Failed to request current Steam user stats.");
    }

}

std::optional<uint64_t> SteamAPI::GetSteamID64() const noexcept
{
    return steamID;
}

bool SteamAPI::RequestCurrentStats() const
{
    if (!RequestCurrentStatsFn || !pSteamUserStats)
        return false;
    return RequestCurrentStatsFn(pSteamUserStats);
}

bool SteamAPI::SetAchievement(const char* achievementID) const
{
    if (!SetAchievementFn || !pSteamUserStats)
        return false;
    return SetAchievementFn(pSteamUserStats, achievementID);
}

bool SteamAPI::ClearAchievement(const char* achievementID) const
{
    if (!ClearAchievementFn || !pSteamUserStats)
        return false;
    return ClearAchievementFn(pSteamUserStats, achievementID);
}

bool SteamAPI::StoreStats() const
{
    if (!StoreStatsFn || !pSteamUserStats)
        return false;
    return StoreStatsFn(pSteamUserStats);
}

bool SteamAPI::GetAchievement(const char* achievementID, bool& achieved) const
{
    if (!GetAchievementFn || !pSteamUserStats)
        return false;
    return GetAchievementFn(pSteamUserStats, achievementID, &achieved);
}
