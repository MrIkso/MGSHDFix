#pragma once
#include <optional>
#include <cstdint>
#include <Windows.h>

class SteamAPI final
{
public:
    void Setup();
    void FetchAndCacheSteamID();

    [[nodiscard]] std::optional<uint64_t> GetSteamID64() const noexcept;

    // Achievement-related wrappers
    [[nodiscard]] bool RequestCurrentStats() const;
    [[nodiscard]] bool SetAchievement(const char* achievementID) const;
    [[nodiscard]] bool ClearAchievement(const char* achievementID) const;
    [[nodiscard]] bool StoreStats() const;
    [[nodiscard]] bool GetAchievement(const char* achievementID, bool& achieved) const;

private:
    // SteamUser and SteamUserStats API function typedefs
    using SteamAPI_SteamUser_v021_t = void* (__cdecl*)();
    using SteamAPI_ISteamUser_GetSteamID_t = uint64_t(__cdecl*)(void*);

    // SteamUserStats interface functions:
    using SteamAPI_SteamUserStats_v012_t = void* (__cdecl*)();
    using SteamAPI_ISteamUserStats_RequestCurrentStats_t = bool(__cdecl*)(void*);
    using SteamAPI_ISteamUserStats_SetAchievement_t = bool(__cdecl*)(void*, const char*);
    using SteamAPI_ISteamUserStats_ClearAchievement_t = bool(__cdecl*)(void*, const char*);
    using SteamAPI_ISteamUserStats_StoreStats_t = bool(__cdecl*)(void*);
    using SteamAPI_ISteamUserStats_GetAchievement_t = bool(__cdecl*)(void*, const char*, bool*);

    HMODULE hSteamAPI { nullptr };

    SteamAPI_SteamUser_v021_t SteamUserFn { nullptr };
    SteamAPI_ISteamUser_GetSteamID_t GetSteamIDFn { nullptr };

    SteamAPI_SteamUserStats_v012_t SteamUserStatsFn { nullptr };
    SteamAPI_ISteamUserStats_RequestCurrentStats_t RequestCurrentStatsFn { nullptr };
    SteamAPI_ISteamUserStats_SetAchievement_t SetAchievementFn { nullptr };
    SteamAPI_ISteamUserStats_ClearAchievement_t ClearAchievementFn { nullptr };
    SteamAPI_ISteamUserStats_StoreStats_t StoreStatsFn { nullptr };
    SteamAPI_ISteamUserStats_GetAchievement_t GetAchievementFn { nullptr };

    void* pSteamUser { nullptr };
    void* pSteamUserStats { nullptr };
    std::optional<uint64_t> steamID;
};


inline SteamAPI g_SteamAPI;
