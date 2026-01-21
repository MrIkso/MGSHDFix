#include "stdafx.h"
#include "mgs2_restore_dogtags.hpp"

#include "common.hpp"
#include "logging.hpp"

void MGS2_RestoreDogtags::Initialize()
{
    if (!(eGameType & MGS2))
    {
        return;
    }

    if (!isEnabled)
    {
        SPDLOG_INFO("MGS 2: Restore Dog Tags: Disabled in config, skipping.");
        return;
    }

    if (uintptr_t MGS2_GekkoDogtagResult = (uintptr_t)Memory::PatternScan(baseModule, "47 65 6B 6B 6F 00", "MGS 2: Gekko -> Gackt dogtag"))
    {
        Memory::PatchBytes(MGS2_GekkoDogtagResult, "\x47\x61\x63\x6B\x74", 5);
        spdlog::info("MGS 2: Gekko -> Gackt dogtag: Patch successful.");
    }



}
