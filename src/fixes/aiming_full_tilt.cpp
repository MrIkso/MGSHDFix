#include "common.hpp"
#include "aiming_full_tilt.hpp"
#include "logging.hpp"
#include "steamworks_api.hpp"

void FixAimingFullTilt::Initialize()
{
    if (!(eGameType & MGS2) || !g_FixAimingFullTilt.bEnabled)
    {
        return;
    }

    if (!g_SteamAPI.bIsLegitCopy)
    {
        spdlog::warn("MGS 2: Aiming Full Tilt - Unable to load SteamInput API due to non-legitimate copy. Skipping.");
        return;
    }

    MAKE_HOOK_MID(baseModule, "0F 2F 3D ?? ?? ?? ?? 41 0F 47 F6", "MGS 2: Aiming Full Tilt", {
        if (ctx.xmm7.f32[0] > 0.99f)
        {
            if (*g_SteamAPI.iNumberOfControllers > 0)
            {
                const InputHandle_t controller = g_SteamAPI.controllerHandles[0];
                InputDigitalActionData_t lockOnData = SteamInput()->GetDigitalActionData(controller, g_SteamAPI.hL1Button);
                if (lockOnData.bState)
                {
                    ctx.xmm7.f32[0] = 0.99f;
                }
            }
        }
        });

}
