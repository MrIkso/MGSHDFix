#pragma once
#include "helper.hpp"

#include <map>

inline std::string sExeName;
inline std::filesystem::path sExePath;
inline std::filesystem::path sGameSavePath;

inline HMODULE baseModule = GetModuleHandle(NULL);
inline HMODULE engineModule;
inline HMODULE unityPlayer;


struct GameInfo
{
    std::string GameTitle;
    std::string ExeName;
    int SteamAppId;
};
inline const GameInfo* game = nullptr;

enum MgsGame : std::uint8_t
{
    NONE     = 0,
    MGS2     = 1 << 0,
    MGS3     = 1 << 1,
    MG       = 1 << 2,
    LAUNCHER = 1 << 3,
    UNKNOWN  = 1 << 4
};
inline MgsGame eGameType = UNKNOWN;

inline const std::map<MgsGame, GameInfo> kGames = {
    {MGS2, {"Metal Gear Solid 2 MC", "METAL GEAR SOLID2.exe", 2131640}},
    {MGS3, {"Metal Gear Solid 3 MC", "METAL GEAR SOLID3.exe", 2131650}},
    {MG, {"Metal Gear / Metal Gear 2 (MSX)", "METAL GEAR.exe", 2131680}},
};


enum MGS2WeaponIndex : uint8_t
{
    MGS2_WEAPON_INDEX_M9                   = 0x1,
    MGS2_WEAPON_INDEX_USP                  = 0x2,
    MGS2_WEAPON_INDEX_SOCOM                = 0x3,
    MGS2_WEAPON_INDEX_PSG1                 = 0x4,
    MGS2_WEAPON_INDEX_RGB6                 = 0x5,
    MGS2_WEAPON_INDEX_NIKITA               = 0x6,
    MGS2_WEAPON_INDEX_STINGER              = 0x7,
    MGS2_WEAPON_INDEX_CLAYMORE             = 0x8,
    MGS2_WEAPON_INDEX_C4                   = 0x9,
    MGS2_WEAPON_INDEX_CHAFF_GRENADE        = 0xA,
    MGS2_WEAPON_INDEX_STUN_GRENADE         = 0xB,
    MGS2_WEAPON_INDEX_D_MIC                = 0xC,
    MGS2_WEAPON_INDEX_HIGH_FREQUENCY_BLADE = 0xD,
    MGS2_WEAPON_INDEX_COOLANT              = 0xE,
    MGS2_WEAPON_INDEX_AKS74U               = 0xF,
    MGS2_WEAPON_INDEX_MAGAZINE             = 0x10,
    MGS2_WEAPON_INDEX_GRENADE              = 0x11,
    MGS2_WEAPON_INDEX_M4                   = 0x12,
    MGS2_WEAPON_INDEX_PSG1T                = 0x13,
    MGS2_WEAPON_INDEX_D_MIC_ZOOMED         = 0x14,
    MGS2_WEAPON_INDEX_BOOK                 = 0x15,
};

enum MGS3WeaponIndex : uint8_t
{   
    MGS3_WEAPON_INDEX_MK22              = 0x5,
    MGS3_WEAPON_INDEX_M1911A1           = 0x6,
    MGS3_WEAPON_INDEX_EzGun             = 0x7,
    MGS3_WEAPON_INDEX_SAA               = 0x8,
    MGS3_WEAPON_INDEX_Patriot           = 0x9,
    MGS3_WEAPON_INDEX_Scorpion          = 0xA, 
    MGS3_WEAPON_INDEX_XM16E1            = 0xB, 
    MGS3_WEAPON_INDEX_AK47              = 0xC, 
    MGS3_WEAPON_INDEX_M63               = 0xD, 
    MGS3_WEAPON_INDEX_M37               = 0xE, 
    MGS3_WEAPON_INDEX_SVD               = 0xF, 
    MGS3_WEAPON_INDEX_Mosin             = 0x10, 
    MGS3_WEAPON_INDEX_RPG7              = 0x11,

    /*
    MGS3_WEAPON_INDEX_SurvivalKnife     = 0x,
    MGS3_WEAPON_INDEX_Fork              = 0x,
    MGS3_WEAPON_INDEX_CigSpray          = 0x,
    MGS3_WEAPON_INDEX_Handkerchief      = 0x,
    MGS3_WEAPON_INDEX_Torch             = 0x,
    MGS3_WEAPON_INDEX_Grenade           = 0x,
    MGS3_WEAPON_INDEX_WpGrenade         = 0x,
    MGS3_WEAPON_INDEX_StunGrenade       = 0x,
    MGS3_WEAPON_INDEX_ChaffGrenade      = 0x,
    MGS3_WEAPON_INDEX_SmokeGrenade      = 0x,
    MGS3_WEAPON_INDEX_EmptyMag          = 0x,
    MGS3_WEAPON_INDEX_TNT               = 0x,
    MGS3_WEAPON_INDEX_C3                = 0x,
    MGS3_WEAPON_INDEX_Claymore          = 0x,
    MGS3_WEAPON_INDEX_Book              = 0x,
    MGS3_WEAPON_INDEX_Mousetrap         = 0x,
    MGS3_WEAPON_INDEX_DirectionalMic    = 0x,
    */
};

