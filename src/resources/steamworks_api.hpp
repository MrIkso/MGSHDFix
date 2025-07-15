#pragma once

#include <optional>
#include <cstdint>

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

    bool bIsLegitCopy = true;
    bool bIsOnline = true;

private:
    std::optional<uint64_t> steamID;
};

inline SteamAPI g_SteamAPI;
