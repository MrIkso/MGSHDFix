#include "common.hpp"
#include "keep_aiming_after_firing.hpp"

#include "gamevars.hpp"
#include "logging.hpp"
#include "steamworks_api.hpp"

/// Originally made by Zenf as part of the Keep Aiming mod for MGS3.


void KeepAimingAfterFiring::Initialize()
{
    if (!(eGameType & (MGS2|MGS3)) || !(g_KeepAimingAfterFiring.bAlwaysKeepAiming || g_KeepAimingAfterFiring.bKeepAimingOnR1Held || g_KeepAimingAfterFiring.bKeepAimingOnL1Held))
    {
        return;
    }

    if (!g_SteamAPI.bIsLegitCopy && !g_KeepAimingAfterFiring.bAlwaysKeepAiming)
    {
        spdlog::warn("MGS 2/3: Keep Aiming After Firing - Unable to load SteamInput API due to non-legitimate copy. Keep Aiming R1/L1 funtionality not avaiaible.");
        return;
    }

    if (eGameType & MGS2)
    {
        MAKE_HOOK_MID(baseModule, "4C 89 25 ?? ?? ?? ?? EB", "MGS 2: Keep Aiming After Firing", {
            switch (ctx.rsi)
            {
                case MGS2_WEAPON_INDEX_AKS74U:
                case MGS2_WEAPON_INDEX_M4:
                    break;
                default:
                    return;
            }
            if (g_KeepAimingAfterFiring.bAlwaysKeepAiming)
            {
                ctx.r12 = g_GameVars.GetAimingState();
                return;
            }
            if (*g_SteamAPI.iNumberOfControllers > 0)
            {
                const InputHandle_t controller = g_SteamAPI.controllerHandles[0];
                if (g_KeepAimingAfterFiring.bKeepAimingOnR1Held)
                {
                    InputDigitalActionData_t R1Data = SteamInput()->GetDigitalActionData(controller, g_SteamAPI.hR1Button);
                    if (R1Data.bState)
                    {
                        ctx.r12 = g_GameVars.GetAimingState();
                        return;
                    }
                }
                if (g_KeepAimingAfterFiring.bKeepAimingOnL1Held)
                {
                    InputDigitalActionData_t L1Data = SteamInput()->GetDigitalActionData(controller, g_SteamAPI.hL1Button);
                    if (L1Data.bState)
                    {
                        ctx.r12 = g_GameVars.GetAimingState();
                        return;
                    }
                }
            
            }
        });
    }
    else if (eGameType & MGS3)
    {
        MAKE_HOOK_MID(baseModule, "48 89 1D ?? ?? ?? ?? 4C 8D 15", "MGS 3: Keep Aiming After Firing", {
            switch (ctx.r9)
            {
                case MGS3_WEAPON_INDEX_MK22:
                case MGS3_WEAPON_INDEX_M1911A1:
                case MGS3_WEAPON_INDEX_EzGun:
                case MGS3_WEAPON_INDEX_SAA:
                case MGS3_WEAPON_INDEX_Patriot:
                case MGS3_WEAPON_INDEX_Scorpion:
                case MGS3_WEAPON_INDEX_XM16E1:
                case MGS3_WEAPON_INDEX_AK47:
                case MGS3_WEAPON_INDEX_M63:
                case MGS3_WEAPON_INDEX_M37:
                    break;
                default:
                    return;
            }
            if (g_KeepAimingAfterFiring.bAlwaysKeepAiming)
            {
                ctx.r9 = g_GameVars.GetAimingState();
                return;
            }
            if (*g_SteamAPI.iNumberOfControllers > 0)
            {
                const InputHandle_t controller = g_SteamAPI.controllerHandles[0];
                if (g_KeepAimingAfterFiring.bKeepAimingOnR1Held)
                {
                    InputDigitalActionData_t R1Data = SteamInput()->GetDigitalActionData(controller, g_SteamAPI.hR1Button);
                    if (R1Data.bState)
                    {
                        ctx.rbx = g_GameVars.GetAimingState();
                        return;
                    }
                }
                if (g_KeepAimingAfterFiring.bKeepAimingOnL1Held)
                {
                    InputDigitalActionData_t L1Data = SteamInput()->GetDigitalActionData(controller, g_SteamAPI.hL1Button);
                    if (L1Data.bState)
                    {
                        ctx.rbx = g_GameVars.GetAimingState();
                        return;
                    }
                }
            }
        });
    }
}


