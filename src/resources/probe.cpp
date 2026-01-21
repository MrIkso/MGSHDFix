#include "stdafx.h"

/*#if !defined(RELEASE_BUILD)
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
    { "81 48 ?? ?? ?? ?? ?? 3B 96",             7 }, //lod stuff here
      { "81 ?? ?? ?? 00 00 00 30 00 00",   10 },
      {"81 ?? ?? ?? ?? 00 00 00 30 00 00",11},

      //not unique / needs patterns
     { "41 81 48 ?? ?? ?? ?? ?? 48 8B 87",  8 },
      {"41 81 48 ?? ?? ?? ?? ?? F6 87",  8 },
      {"41 81 E0 ?? ?? ?? ?? 89 41", 7},

      //need to do in sequence
      {"41 81 C9 00 30 00 00", 7},
      {"81 C9 00 30 00 00", 6}

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
            if (count > 296)
            {
                break;
            }
            spdlog::info("   Count: {} Found at: 0x{:X} - patching {} bytes", count, result, length);

            std::string nops = GenerateNops(length);
            Memory::PatchBytes(result, nops.c_str(), length);

            spdlog::info("Patched with NOPs.");
        }
    }

    spdlog::info("[Skybox] Scan & patch completed.");
}
#endif

#if !defined(RELEASE_BUILD)
ScanAndPatchSkybox();
#endif

/*
if (!(eGameType & MGS2))
{
    return;
}

if (uint8_t* grassTest1_Result = Memory::PatternScan(baseModule, "8B D0 3D ?? ?? ?? ?? 7E", "mgs2 dd"))
{
    static SafetyHookMid grassTest1MidHook {};
    grassTest1MidHook = safetyhook::create_mid(grassTest1_Result,
        [](SafetyHookContext& ctx)
        {
            ctx.rax = 300; // Set the distance culling value
            ctx.rflags |= 0x0000000000000002; // Set the ZF flag to 1
        });
    LOG_HOOK(grassTest1MidHook, "grass scan")
}

if (isEnabled || !(eGameType & MGS3))
{
    return;

}


if (uint8_t* grassTest1_Result = Memory::PatternScan(baseModule, "49 8B 54 2E ?? 0F 85", "grass scan"))
{
    static SafetyHookMid grassTest1MidHook {};
    grassTest1MidHook = safetyhook::create_mid(grassTest1_Result,
        [](SafetyHookContext& ctx)
        {
            ctx.xmm0.f32[0] = 400.0f; // Set the distance culling value
        });
    LOG_HOOK(grassTest1MidHook, "grass scan")
}

*/

/*

if (uint8_t* lodtestResult = Memory::PatternScan(baseModule, "41 F7 C2 ?? ?? ?? ?? 75", "MGS 2: model test"))
{

        static SafetyHookMid lodtest_MidHook {};
        lodtest_MidHook = safetyhook::create_mid(lodtestResult,
            [](SafetyHookContext& ctx)
            {
                reghelpers::set_r10d(ctx, reghelpers::get_r10d(ctx) & 0x2);
                //spdlog::info("r10d: {}", reghelpers::get_r10d(ctx));
            });
        LOG_HOOK(lodtest_MidHook, "MGS 2: Effect Speed Fix: effect3\\flying_smoke_slow.c")
}

/* ALWAYS LOW POLY
if (uint8_t* lodtestResult = Memory::PatternScan(baseModule, "48 89 5C 24 ?? 41 F7 C2", "MGS 2: model test"))
{

    static SafetyHookMid lodtest_MidHook {};
    lodtest_MidHook = safetyhook::create_mid(lodtestResult,
        [](SafetyHookContext& ctx)
        {
            reghelpers::set_r12d(ctx, 1);
        });
    LOG_HOOK(lodtest_MidHook, "MGS 2: Effect Speed Fix: effect3\\flying_smoke_slow.c")
}
*/

/*
    MAKE_HOOK_MID(baseModule, "E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 0F 84 ?? ?? ?? ?? 4C 8D 05 ?? ?? ?? ?? 48 8B C8 48 8D 15 ?? ?? ?? ?? E8 ?? ?? ?? ?? 0F 57 C0", "flag 1", {
        spdlog::info("hook 1 hit");
        });

    MAKE_HOOK_MID(baseModule, "48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC ?? 45 33 C9 8B FA 8B F1 BA ?? ?? ?? ?? 41 B8 ?? ?? ?? ?? 41 8D 49 ?? E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 74 ?? 4C 8D 05 ?? ?? ?? ?? 48 8B C8 48 8D 15 ?? ?? ?? ?? E8 ?? ?? ?? ?? 44 8B C7 8B D6 48 8B CB E8 ?? ?? ?? ?? 85 C0 79 ?? 48 8B CB E8 ?? ?? ?? ?? 33 C0 48 8B 5C 24 ?? 48 8B 74 24 ?? 48 83 C4 ?? 5F C3 48 8B 74 24 ?? 48 8B C3 48 8B 5C 24 ?? 48 83 C4 ?? 5F C3 CC CC CC CC CC CC CC CC CC CC 40 57", "flag 2", {
    spdlog::info("hook 2 hit");
        });*/