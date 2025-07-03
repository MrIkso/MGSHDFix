#include "aiming_after_equip.hpp"

#include <spdlog/spdlog.h>

#include "common.hpp"

/*
if ((eGameType == MgsGame::MGS2) || (eGameType == MgsGame::MGS3))
{
    uint8_t* WeaponPutawayScanResult = Memory::PatternScan(baseModule, ((eGameType == MgsGame::MGS2) ? "89 1D ?? ?? ?? ?? 8B C3 48 83 C4" : nullptr));
    if (WeaponPutawayScanResult)
    {
        // uint8_t* Weapon_Aiming_Origin_Offset = Memory::PatternScan(baseModule, "83 3D ?? ?? ?? ?? 00 ?? ?? F2 0F 10 0D");
         //int* is_aiming = reinterpret_cast<int*>(Memory::GetAbsolute((uintptr_t)Weapon_Aiming_Origin_Offset + 13));

        static SafetyHookMid WeaponPutawayFix4MidHook {};
        WeaponPutawayFix4MidHook = safetyhook::create_mid(WeaponPutawayScanResult,
            [](SafetyHookContext& ctx)
            {
                spdlog::info("MGS2 | MGS3: Unequip pointer 4");
                MGS2_MGS3_Aiming_Fix();
            });
    }

    void MGS2_MGS3_Aiming_Fix()
    {
        spdlog::info("MGS2 | MGS3: triggered");
        return;
    }
    */

void AimAfterEquipFix::Initialize() const
{
    if (!(eGameType & (MGS2 | MGS3)))
    {
        return;
    }
    /* TEMPLATE CODE. DON'T ACTIVATE.
    if (uint8_t* weaponPutawayScanResult = Memory::PatternScan(baseModule, "89 1D ?? ?? ?? ?? 8B C3 48 83 C4", "MGS2 | MGS3: Aiming After Equip", nullptr, nullptr))
    {
        static SafetyHookMid WeaponPutawayFixMidHook {};
        WeaponPutawayFixMidHook = safetyhook::create_mid(weaponPutawayScanResult,
            [](SafetyHookContext& ctx)
            {
                spdlog::info("MGS2 | MGS3: Weapon putaway detected, applying aiming fix");
            });
        LOG_HOOK(WeaponPutawayFixMidHook, "MGS2 | MGS3: Aiming After Equip", NULL, NULL)
    }*/
}
