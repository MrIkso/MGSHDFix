#include "mute_warning.hpp"
#include "common.hpp"
#include "spdlog/spdlog.h"

void MuteWarning::Initialize() const
{
    if (!(eGameType & (MG | MGS2 | MGS3)))
    {
        return;
    }

    if (uint8_t* muteWarningResult = Memory::PatternScan(baseModule, "89 05 ?? ?? ?? ?? 89 11", "MG-MG2 | MGS 2 | MGS 3: Mute Warning", nullptr, nullptr))
    {
        static SafetyHookMid MuteWarningMidHook {};
        MuteWarningMidHook = safetyhook::create_mid(muteWarningResult,
            [](SafetyHookContext& ctx)
            {
                if (ctx.rdx == 1)
                {
                    spdlog::warn("MG-MG2 | MGS 2 | MGS 3: Game audio is currently muted via the main launcher!");
                }
            });
        LOG_HOOK(MuteWarningMidHook, "MG-MG2 | MGS 2 | MGS 3: Mute Warning", NULL, NULL)
    }
}
