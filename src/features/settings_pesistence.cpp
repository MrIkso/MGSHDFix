#include "stdafx.h"
#include "settings_pesistence.hpp"

#include "logging.hpp"







void SettingsPersistence::Initialize()
{
    if (!bEnabled)
    {
        return;
    }
    spdlog::info("Settings Persistence: Initialized.");
}
