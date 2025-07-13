#pragma once
#include <cstdint>

class MuteWarning
{
private:
    uintptr_t muteWarningAddress = 0;

public:
    void Setup();
    void CheckStatus() const;
    bool bEnabled;
};

inline MuteWarning g_MuteWarning;


