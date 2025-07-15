#include "common.hpp"
#include "steamworks_api.hpp"
#include <logging.hpp>


#pragma warning(push)
#pragma warning(disable:4828)
#include <isteamuser.h>
#include <isteamuserstats.h>
#pragma warning(pop)

#pragma comment(lib,"steam_api64.lib")

void SteamAPI::Setup()
{
    if (!bIsLegitCopy)
    {
        spdlog::warn("Steam achievement/stat tracking fixes are disabled due to non-legitimate copy.");
        return;
    }

    if (!(eGameType & (MG|MGS2|MGS3)))
    {
        return;
    }

    if (uint8_t* AfterSteamSetupResult = Memory::PatternScan(baseModule, eGameType & MGS2 ? "48 8B 05 ?? ?? ?? ?? 8B CB" : (eGameType & MGS3 ? "48 8B 05 ?? ?? ?? ?? 8B CF 83 78" : "48 8B 05 ?? ?? ?? ?? 8B CF"), "Steam API Initialization"))
    {
        static SafetyHookMid SteamMidhook {};
        SteamMidhook = safetyhook::create_mid(AfterSteamSetupResult,
            [](SafetyHookContext& ctx)
            {
                g_SteamAPI.FetchAndCacheSteamID();
            });
        LOG_HOOK(SteamMidhook, "SteamAPI Initialization")
    }
}

void SteamAPI::FetchAndCacheSteamID()
{
    bIsOnline = SteamUser()->BLoggedOn();
    if (!bIsOnline)
    {
        spdlog::warn("SteamAPI - Not logged in or offline.");
        return;
    }
    CSteamID mySteamID = SteamUser()->GetSteamID();
    steamID = mySteamID.ConvertToUint64();
    spdlog::info("----------");
    spdlog::info("SteamAPI - Is Online: {}", bIsOnline ? "Yes" : "No");
    spdlog::info("SteamAPI - SteamID: {}", *steamID);
    spdlog::info("----------");
}

std::optional<uint64_t> SteamAPI::GetSteamID64() const noexcept
{
    return steamID;
}

bool SteamAPI::RequestCurrentStats() const
{
    return SteamUserStats()->RequestCurrentStats();
}

bool SteamAPI::SetAchievement(const char* achievementID) const
{
    return SteamUserStats()->SetAchievement(achievementID);
}

bool SteamAPI::ClearAchievement(const char* achievementID) const
{
    return SteamUserStats()->ClearAchievement(achievementID);
}

bool SteamAPI::StoreStats() const
{
    return SteamUserStats()->StoreStats();
}

bool SteamAPI::GetAchievement(const char* achievementID, bool& achieved) const
{
    return SteamUserStats()->GetAchievement(achievementID, &achieved);
}
