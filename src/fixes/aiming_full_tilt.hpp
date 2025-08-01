#pragma once

#pragma warning(push)
#pragma warning(disable:4828)
#include <isteaminput.h>
#pragma warning(pop)

class FixAimingFullTilt final
{
public:
    static void Initialize();
    bool bEnabled = true;
    
};

inline FixAimingFullTilt g_FixAimingFullTilt;
