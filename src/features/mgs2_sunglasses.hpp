#pragma once

class MGS2Sunglasses final
{
private:

    uint8_t* aCompletionCheckResult = nullptr;
    uint8_t* aCompletionCountResult = nullptr;
    uint8_t* aIroquiosNPCResult = nullptr;
    static bool ShouldTriggerSunglasses();
public:
    static void Initialize();

    static void CheckOnTransition();


    bool bEnabled = false;
    bool bAlwaysWearingSunglasses = false;
};

inline MGS2Sunglasses g_MGS2Sunglasses;
