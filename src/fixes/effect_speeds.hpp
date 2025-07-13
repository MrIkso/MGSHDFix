#pragma once
#include <chrono>

class EffectSpeedFix
{
public:
    bool isEnabled = true;
    std::chrono::time_point<std::chrono::high_resolution_clock> solidusDashAct_NextUpdate;
    double iExplosionDuration = 0;
    int iDebrisIteration = 0;
    void Initialize() const;
    void Reset();
};

inline EffectSpeedFix g_EffectSpeedFix;


