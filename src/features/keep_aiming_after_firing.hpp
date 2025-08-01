#pragma once

class KeepAimingAfterFiring final
{
public:
    static void Initialize();

    bool bAlwaysKeepAiming;
    bool bKeepAimingOnR1Held;
    bool bKeepAimingOnL1Held;
};

inline KeepAimingAfterFiring g_KeepAimingAfterFiring;
