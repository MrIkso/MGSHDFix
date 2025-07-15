#pragma once

class StereoAudioFix final
{
public:
    bool isEnabled;
    void Initialize() const;
};

inline StereoAudioFix g_StereoAudioFix;
