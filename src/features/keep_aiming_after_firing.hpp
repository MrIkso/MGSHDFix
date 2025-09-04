#pragma once

class KeepAimingAfterFiring final
{
public:
    static void Initialize();

    bool bAlwaysKeepAiming;
    bool bKeepAimingInFirstPerson;
    bool bKeepAimingOnLockOn;
    bool bOverrodeState = false;
};

inline KeepAimingAfterFiring g_KeepAimingAfterFiring;
