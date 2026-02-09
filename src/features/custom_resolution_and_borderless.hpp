#pragma once

namespace CustomResolutionAndBorderless
{
    void Init_FixDPIScaling();
    void Init_CalculateScreenSize();
    void Init_CustomResolution();
    void Init_ScaleEffects();
    void Init_AspectFOVFix();
    void Init_HUDFix();

    inline bool bAspectFix;

    inline bool bHUDFix;
    inline bool bFOVFix;
    inline bool bOutputResolution;
    inline int iOutputResX;
    inline int iOutputResY;
    inline int iInternalResX;
    inline int iInternalResY;
    inline bool bWindowedMode;
    inline bool bBorderlessMode;
    inline bool bFramebufferFix;
}

