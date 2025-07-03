#pragma once

class PauseOnFocusLoss
{
private:
public:
    bool bPauseOnFocusLoss;
    bool bSpeedrunnerBugfixOverride;
    void Initialize();
    bool ShouldFixPauseState();
    
};

inline PauseOnFocusLoss g_PauseOnFocusLoss;


