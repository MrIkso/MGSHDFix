#include "common.hpp"

#include "mgs2_sunglasses.hpp"

#include "logging.hpp"
#include "steamworks_api.hpp"
#include <random>

bool MGS2Sunglasses::ShouldTriggerSunglasses()
{
    // Get current system time (local)
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm localTime {};
#ifdef _WIN32
    localtime_s(&localTime, &now_c);
#else
    localtime_r(&now_c, &localTime);
#endif

    int hour = localTime.tm_hour;
    if (hour < 6 || hour >= 19)
    {
        return false;
    }

    thread_local std::mt19937 rng(std::random_device {}());
    std::uniform_int_distribution<int> chanceDist(1, 3);
    std::uniform_int_distribution<int> rollDist(0, 99);
    int threshold = chanceDist(rng);
    int roll = rollDist(rng);
    return roll < threshold;
}



void MGS2Sunglasses::CheckOnTransition()
{
    static bool bSunglassesTriggered = false;
    if (g_SteamAPI.bIsLegitCopy || g_MGS2Sunglasses.bAlwaysWearingSunglasses || bSunglassesTriggered)
    {
        return;
    }

    if (ShouldTriggerSunglasses())
    {
        bSunglassesTriggered = true;
        Memory::PatchBytes(reinterpret_cast<uintptr_t>(g_MGS2Sunglasses.aCompletionCheckResult), "\x90\x90", 2);
        Memory::PatchBytes(reinterpret_cast<uintptr_t>(g_MGS2Sunglasses.aCompletionCountResult), "\x90\x90", 2);
        Memory::PatchBytes(reinterpret_cast<uintptr_t>(g_MGS2Sunglasses.aIroquiosNPCResult), "\x90\x90", 2);
    }
}

void MGS2Sunglasses::Initialize()
{
    if (!(eGameType & MGS2))
    {
        return;
    }

    if (!g_MGS2Sunglasses.bEnabled && g_SteamAPI.bIsLegitCopy)
    {
        return;
    }


    if (g_MGS2Sunglasses.bAlwaysWearingSunglasses || !g_SteamAPI.bIsLegitCopy)
    {
        if (g_MGS2Sunglasses.aCompletionCheckResult = Memory::PatternScan(baseModule, "74 ?? B9 ?? ?? ?? ?? BA ?? ?? ?? ?? 44 8D 49", "completion check 1"); g_MGS2Sunglasses.bAlwaysWearingSunglasses)
        {
            Memory::PatchBytes(reinterpret_cast<uintptr_t>(g_MGS2Sunglasses.aCompletionCheckResult), "\x90\x90", 2);
            spdlog::info("MGS 2 Sunglasses: Patched Completion Check.");
        }

        if (g_MGS2Sunglasses.aCompletionCountResult = Memory::PatternScan(baseModule, "75 ?? ?? ?? ?? ?? ?? 81 4B", "completion count"); g_MGS2Sunglasses.bAlwaysWearingSunglasses)
        {
            Memory::PatchBytes(reinterpret_cast<uintptr_t>(g_MGS2Sunglasses.aCompletionCountResult), "\x90\x90", 2);
            spdlog::info("MGS Sunglasses: Patched Completion Count.");
        }

        if (g_MGS2Sunglasses.aIroquiosNPCResult = Memory::PatternScan(baseModule, "74 ?? 45 33 C9 C7 44 24 ?? ?? ?? ?? ?? 41 B8 ?? ?? ?? ?? 48 8D 96", "Iroquios NPC"); g_MGS2Sunglasses.bAlwaysWearingSunglasses)
        {
            Memory::PatchBytes(reinterpret_cast<uintptr_t>(g_MGS2Sunglasses.aIroquiosNPCResult), "\x90\x90", 2);
            spdlog::info("MGS 2 Sunglasses: Patched Iroquios NPC.");
        }
    }
    else //should never have sunglasses.
    {
        MAKE_HOOK_MID(baseModule, "48 85 C0 0F 84 ? ? ? ? 45 33 C0 BA", "MGS2: Sunglasses - Never", {
            ctx.rax = 0;
            spdlog::info("MGS 2 Sunglasses: Patched Never Wearing Sunglasses.");
        });
    }

}
