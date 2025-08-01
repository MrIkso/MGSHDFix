#pragma once

class KeepAimingAfterFiring final
{
public:
    static void Initialize();

    bool bAlwaysKeepAiming;
    bool bKeepAimingInFirstPerson;
    bool bKeepAimingOnLockOn;
};

inline KeepAimingAfterFiring g_KeepAimingAfterFiring;
