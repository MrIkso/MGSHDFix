#pragma once

namespace DamagedSaveFix
{
    void Initialize();

    inline bool bEnabled = true;
    inline bool bEnableConsoleNotification = true;
    inline bool bDeleteOutdatedSaveData = false; //if damaged save data should be moved to a backup folder, or outright deleted.
};

