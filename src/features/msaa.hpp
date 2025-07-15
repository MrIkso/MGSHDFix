#pragma once

class MultiSampleAntiAliasing final
{
public:
    bool isEnabled;
    void Initialize() const;
};

inline MultiSampleAntiAliasing g_MultiSampleAntiAliasing;


