#include "stdafx.h"
#include "common.hpp"
#include "aiming_full_tilt.hpp"

#include "gamevars.hpp"
#include "logging.hpp"
#include "steamworks_api.hpp"

void FixAimingFullTilt::Initialize()
{
    if (!(eGameType & MGS2) || !bEnabled)
    {
        return;
    }

    MAKE_HOOK_MID(baseModule, "0F 2F 3D ?? ?? ?? ?? 41 0F 47 F6", "MGS 2: Aiming Full Tilt", {
        if (ctx.xmm7.f32[0] > 0.99f)
        {
            if(g_GameVars.MGS2IsHoldingLockOn())
            {
                ctx.xmm7.f32[0] = 0.99f;
            }
        }
    });

}
