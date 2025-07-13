#pragma once
#include <cstdint>

class MuteWarning
{
private:
    int* muteWarningAddress = nullptr;

public:
    void Setup();
    void CheckStatus() const;
    bool bEnabled = false;
};

inline MuteWarning g_MuteWarning;


