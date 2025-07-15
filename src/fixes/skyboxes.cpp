#include "skyboxes.hpp"
#include "common.hpp"
#include "logging.hpp"

// cipherxof's Skybox Rendering Fix
void SkyboxFix::Initialize()
{
    if (!(eGameType & MGS2))
    {
        return;
    }

    if (uintptr_t MGS2_CreateSkyUtilScanResult = (uintptr_t)Memory::PatternScan(baseModule, "81 4F ?? ?? 30 00 00 4D 85 FF", "MGS 2: Skybox"))
    {
        Memory::PatchBytes(MGS2_CreateSkyUtilScanResult, "\x90\x90\x90\x90\x90\x90\x90", 7);
        spdlog::info("MGS 2: Skybox: Patch successful.");
    }

}
