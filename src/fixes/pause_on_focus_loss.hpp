#pragma once

class PauseOnFocusLoss
{
private:
public:
    bool bPauseOnFocusLoss;
    void Initialize();
    bool ShouldFixPauseState();
    
};

inline PauseOnFocusLoss g_PauseOnFocusLoss;


