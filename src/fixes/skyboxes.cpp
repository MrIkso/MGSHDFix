#include "skyboxes.hpp"
#include "common.hpp"
#include "spdlog/spdlog.h"
/*
struct SkyboxPattern
{
    const char* pattern;
    size_t length;
};

SkyboxPattern testPatterns[] = {
    //known unique
    { "0D 00 30 00 00",                    5 },
   { "41 81 4A 58 00 30 00 00",          8 },
   { "41 81 ?? ?? 00 00 00 00 30 00 00",11 },
   { "41 81 C8 00 30 00 00",             7 },
   { "41 81 CC 00 30 00 00",             7 },
   { "41 81 CE 00 30 00 00",             7 },
   { "81 CA 00 30 00 00",                6 },
    { "42 81 4C 00 50 00 30 00 00",       9 },
   { "81 08 00 30 00 00",                6 },
   { "81 09 00 30 00 00",                6 },
    { "81 ?? ?? 00 30 00 00",             7 }, //lod stuff here
   { "81 ?? ?? ?? 00 00 00 30 00 00",   10 },
   { "81 ?? ?? ?? ?? 00 00 00 30 00 00",11 },
    
    //not unique / needs patterns
   { "41 81 48 ?? ?? ?? ?? ?? 48 8B 87",  8 },
    //{"41 81 48 ?? ?? ?? ?? ?? F6 87",  8 },
    //{"41 81 E0 ?? ?? ?? ?? 89 41", 7},

    //need to do in sequence
    //{"41 81 C9 00 30 00 00", 7},
    //{"81 C9 00 30 00 00", 6}
    
};

std::string GenerateNops(size_t count)
{
    return std::string(count, '\x90');
}

int count;
void ScanAndPatchSkybox()
{
    const size_t numPatterns = sizeof(testPatterns) / sizeof(testPatterns[0]);
    uintptr_t result = 0;

    for (size_t i = 0; i < numPatterns; ++i)
    {
        const char* pattern = testPatterns[i].pattern;
        size_t length = testPatterns[i].length;

        spdlog::info("[Skybox] Scanning for pattern {}: \"{}\"", i, pattern);
        
        count = 0;
        while ((result = (uintptr_t)Memory::PatternScanSilent(baseModule, pattern)))
        {
            count++;
            spdlog::info("   Count: {} Found at: 0x{:X} - patching {} bytes", count, result, length);

            std::string nops = GenerateNops(length);
            Memory::PatchBytes(result, nops.c_str(), length);

            spdlog::info("Patched with NOPs.");
        }
    }

    spdlog::info("[Skybox] Scan & patch completed.");
}*/

// cipherxof's Skybox Rendering Fix
void SkyboxFix::Initialize()
{
    if (!(eGameType & MGS2))
    {
        return;
    }

    if (uintptr_t MGS2_CreateSkyUtilScanResult = (uintptr_t)Memory::PatternScan(baseModule, "81 4F ?? ?? 30 00 00 4D 85 FF", "MGS 2: Skybox", NULL, NULL))
    {
        Memory::PatchBytes(MGS2_CreateSkyUtilScanResult, "\x90\x90\x90\x90\x90\x90\x90", 7);
        spdlog::info("MGS 2: Skybox: Patch successful.");
    }

    //ScanAndPatchSkybox();


    if (uint8_t* MGS2_flyingSmokeSlowScanResult = Memory::PatternScan(baseModule, "8B D0 3D ?? ?? ?? ?? 7E", "MGS 2: model test", NULL, NULL))
    {
        static SafetyHookMid flyingSmokeSlow_MidHook {};
        flyingSmokeSlow_MidHook = safetyhook::create_mid(MGS2_flyingSmokeSlowScanResult,
            [](SafetyHookContext& ctx)
            {
                
            });
        LOG_HOOK(flyingSmokeSlow_MidHook, "MGS 2: Effect Speed Fix: effect3\\flying_smoke_slow.c", NULL, NULL)
    }


}
