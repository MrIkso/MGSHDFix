#include "stdafx.h"

#include "custom_resolution_and_borderless.hpp"

#include "logging.hpp"

#include "common.hpp"
#include "config.hpp"
#include "d3d11_api.hpp"
#include "mute_warning.hpp"

namespace 
{


    float fAspectRatio;

    // Aspect ratio + HUD stuff
    constexpr float fNativeAspect = 16.0f / 9.0f;
    float fAspectMultiplier;
    float fHUDWidth;
    float fHUDHeight;
    constexpr float fDefaultHUDWidth = 1280;
    constexpr float fDefaultHUDHeight = 720;
    float fHUDWidthOffset;
    float fHUDHeightOffset;
    float fMGS2_EffectScaleX;
    float fMGS2_EffectScaleY;

    // CreateWindowExA Hook
    SafetyHookInline CreateWindowExA_hook {};
    HWND WINAPI CreateWindowExA_hooked(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
    {
        const LPCSTR sClassName = "CSD3DWND";
        if (std::string(lpClassName) == std::string(sClassName))
        {
            if (CustomResolutionAndBorderless::bBorderlessMode && !(eGameType & UNKNOWN))
            {
                auto hWnd = CreateWindowExA_hook.stdcall<HWND>(dwExStyle, lpClassName, lpWindowName, WS_POPUP, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, DesktopDimensions.first, DesktopDimensions.second, NULL);
                spdlog::info("CreateWindowExA: Borderless: ClassName = {}, WindowName = {}, dwStyle = 0x{:08X}, X = {}, Y = {}, nWidth = {}, nHeight = {}", lpClassName, lpWindowName, WS_POPUP, X, Y, nWidth, nHeight);
                spdlog::info("CreateWindowExA: Borderless: SetWindowPos to X = {}, Y = {}, cx = {}, cy = {}", 0, 0, (int)DesktopDimensions.first, (int)DesktopDimensions.second);
                g_D3D11Hooks.MainHwnd = hWnd;
                return hWnd;
            }

            if (CustomResolutionAndBorderless::bWindowedMode && !(eGameType & UNKNOWN))
            {
                auto hWnd = CreateWindowExA_hook.stdcall<HWND>(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, CustomResolutionAndBorderless::iOutputResX, CustomResolutionAndBorderless::iOutputResY, NULL);
                spdlog::info("CreateWindowExA: Windowed: ClassName = {}, WindowName = {}, dwStyle = 0x{:08X}, X = {}, Y = {}, nWidth = {}, nHeight = {}", lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight);
                spdlog::info("CreateWindowExA: Windowed: SetWindowPos to X = {}, Y = {}, cx = {}, cy = {}", 0, 0, CustomResolutionAndBorderless::iOutputResX, CustomResolutionAndBorderless::iOutputResY);
                g_D3D11Hooks.MainHwnd = hWnd;
                return hWnd;
            }
        }

        g_D3D11Hooks.MainHwnd = CreateWindowExA_hook.stdcall<HWND>(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
        return g_D3D11Hooks.MainHwnd;
    }


}

namespace CustomResolutionAndBorderless
{


    void Init_FixDPIScaling()
    {
        if (eGameType & (MG | MGS2 | MGS3))
        {
            SetProcessDPIAware();
            spdlog::info("MG/MG2 | MGS 2 | MGS 3: High-DPI scaling fixed.");
        }
    }


    void Init_CalculateScreenSize()
    {
        // Calculate aspect ratio
        fAspectRatio = (float)iInternalResX / (float)iInternalResY;
        fAspectMultiplier = fAspectRatio / fNativeAspect;

        // HUD variables
        fHUDWidth = iInternalResY * fNativeAspect;
        fHUDHeight = (float)iInternalResY;
        fHUDWidthOffset = (float)(iInternalResX - fHUDWidth) / 2;
        fHUDHeightOffset = 0;
        if (fAspectRatio < fNativeAspect)
        {
            fHUDWidth = (float)iInternalResX;
            fHUDHeight = (float)iInternalResX / fNativeAspect;
            fHUDWidthOffset = 0;
            fHUDHeightOffset = (float)(iInternalResY - fHUDHeight) / 2;
        }


        // Log details about current resolution
        spdlog::info("Current Resolution: Aspect Ratio: {}", fAspectRatio);
        spdlog::info("Current Resolution: Aspect Ratio Multiplier: {}", fAspectMultiplier);
        spdlog::info("Current Resolution: Corrected HUD Width: {}", fHUDWidth);
        spdlog::info("Current Resolution: Correct HUD Height: {}", fHUDHeight);
        spdlog::info("Current Resolution: HUD Width Offset: {}", fHUDWidthOffset);
        spdlog::info("Current Resolution: HUD Height Offset: {}", fHUDHeightOffset);
    }



    void Init_CustomResolution()
    {
        if (eGameType & (MG | MGS2 | MGS3) && bOutputResolution)
        {
            // MGS 2 | MGS 3: Custom Resolution
            uint8_t* MGS2_MGS3_InternalResolutionScanResult = Memory::PatternScan(baseModule, "F2 0F ?? ?? ?? B9 05 00 00 00 E8 ?? ?? ?? ?? 85 ?? 75 ??", "MGS2_MGS3_InternalResolutionScan");
            uint8_t* MGS2_MGS3_OutputResolution1ScanResult = Memory::PatternScan(baseModule, "40 ?? ?? 74 ?? 8B ?? ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? EB ?? B9 06 00 00 00", "MGS2_MGS3_OutputResolution1Scan");
            uint8_t* MGS2_MGS3_OutputResolution2ScanResult = Memory::PatternScan(baseModule, "80 ?? ?? 00 41 ?? ?? ?? ?? ?? 48 ?? ?? ?? BA ?? ?? ?? ?? 8B ??", "MGS2_MGS3_OutputResolution2Scan");
            if (MGS2_MGS3_InternalResolutionScanResult && MGS2_MGS3_OutputResolution1ScanResult && MGS2_MGS3_OutputResolution2ScanResult)
            {
                uint8_t* MGS2_MGS3_FSR_Result = Memory::PatternScanSilent(baseModule, "83 E8 ?? 74 ?? 83 E8 ?? 74 ?? 83 F8 ?? 75 ?? C7 06");

                if (MGS2_MGS3_FSR_Result)
                {
                    static SafetyHookMid FSRWarningMidHook {};
                    FSRWarningMidHook = safetyhook::create_mid(MGS2_MGS3_FSR_Result,
                                                               [](SafetyHookContext& ctx)
                                                               {
                                                                   static bool bFSRWarningShown = false;
                                                                   if (!g_MuteWarning.bEnableFSRWarning || bFSRWarningShown)
                                                                   {
                                                                       return;
                                                                   }
                                                                   bFSRWarningShown = true;
                                                                   spdlog::warn("----------");
                                                                   spdlog::warn("WARNING: The game launcher's built-in AMD FSR Upscaling resolution/graphical options are enabled!");
                                                                   spdlog::warn("WARNING: MGSHDFix already handles increasing the game's resolution.");
                                                                   spdlog::warn("WARNING: Unintended side effects, e.g. pixelization, mipmap issues (oversharpening on textures), and crashing, may occur while the game's built-in settings are enabled!");
                                                                   spdlog::warn("WARNING: It's advised to set both Internal Resolution & Internal Upscaling graphical options in the game's main launcher to default/original unless ABSOLUTELY necessary! (such as some systems crashing during Stillman's tutorial)");
                                                                   spdlog::warn("----------");
                                                                   Logging::ShowConsole();
                                                                   std::cout << "MGSHDFix WARNING\n"
                                                                       "The game launcher's built-in AMD FSR Upscaling resolution/graphical options are enabled!\n"
                                                                       "\n"
                                                                       "MGSHDFix already handles increasing the game's resolution.\n"
                                                                       "Unintended side effects, e.g. pixelization, mipmap issues (oversharpening on textures), and crashing, may occur while the game's built-in settings are enabled!\n"
                                                                       "\n"
                                                                       "It's advised to set both Internal Resolution & Internal Upscaling graphical options in the game's main launcher to default/original unless ABSOLUTELY necessary! (such as some systems crashing during Stillman's tutorial)\n"
                                                                       "\n"
                                                                       "This warning can be muted on the config tool's \"MGSHDfix / Internal\" settings page." << std::endl;
                                                               });

                }

                // Output resolution 1
                static SafetyHookMid OutputResolution1MidHook {};
                OutputResolution1MidHook = safetyhook::create_mid(MGS2_MGS3_OutputResolution1ScanResult,
                                                                  [](SafetyHookContext& ctx)
                                                                  {
                                                                      ctx.rbx = iOutputResX;
                                                                      ctx.rdi = iOutputResY;
                                                                  });

                // Output resolution 2
                static SafetyHookMid OutputResolution2MidHook {};
                OutputResolution2MidHook = safetyhook::create_mid(MGS2_MGS3_OutputResolution2ScanResult,
                                                                  [](SafetyHookContext& ctx)
                                                                  {
                                                                      ctx.r8 = iOutputResX;
                                                                      ctx.r9 = iOutputResY;
                                                                  });

                // Internal resolution
                static SafetyHookMid InternalResolutionMidHook {};
                InternalResolutionMidHook = safetyhook::create_mid(MGS2_MGS3_InternalResolutionScanResult + 0x5,
                                                                   [](SafetyHookContext& ctx)
                                                                   {
                                                                       if (ctx.rbx + 0x4C)
                                                                       {
                                                                           *reinterpret_cast<int*>(ctx.rbx + 0x4C) = iInternalResX;
                                                                           *reinterpret_cast<int*>(ctx.rbx + 0x54) = iInternalResY;

                                                                           *reinterpret_cast<int*>(ctx.rbx + 0x50) = iInternalResX;
                                                                           *reinterpret_cast<int*>(ctx.rbx + 0x58) = iInternalResY;
                                                                       }
                                                                   });

                // Replace loading screens with the appropriate resolutions.
                if (iOutputResY >= 1080)
                {
                    if (!Memory::PatternScanSilent(baseModule, "5F 34 6B 2E 63 74 78 72 00")) //  _4k.ctxr - Make sure the game is a version with 4k loadingscreens
                        spdlog::warn("MGS 2 | MGS 3: Custom Resolution: Splashscreens: Incompatible game version. Skipping.");
                    else
                    {
                        if (uint8_t* MGS2_MGS3_SplashscreenResult = Memory::PatternScan(baseModule, "FF 15 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 4C 8D 44 24 ?? 48 8D 54 24 ?? 48 8B 08 48 8B 01 FF 50 ?? 48 8B 58", "MGS 2 | MGS 3: Custom Resolution: Splashscreens"))
                        {
                            static SafetyHookMid MGS2_MGS3_SplashScreenMidHook {};
                            MGS2_MGS3_SplashScreenMidHook = safetyhook::create_mid(MGS2_MGS3_SplashscreenResult,
                                                                                   [](SafetyHookContext& ctx)
                                                                                   {
                                                                                       std::string fileName = reinterpret_cast<char*>(ctx.rdx);
                                                                                       if (fileName.ends_with("_720.ctxr"))
                                                                                       {
                                                                                           fileName.replace(fileName.end() - 8, fileName.begin(), iOutputResY >= 2160 ? "4k.ctxr" :
                                                                                                            iOutputResY >= 1440 ? "wqhd.ctxr" :
                                                                                                            /*iOutputResY >= 1080*/ "fhd.ctxr");
                                                                                           ctx.rdx = reinterpret_cast<uintptr_t>(fileName.c_str());
                                                                                       }
                                                                                   });
                            LOG_HOOK(MGS2_MGS3_SplashScreenMidHook, "MGS 2 | MGS 3: Custom Resolution")
                        }

                        if (uint8_t* MGS2_MGS3_LoadingScreenEngScanResult = Memory::PatternScan(baseModule, "48 8D 8C 24 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8D 8C 24 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8D 8C 24 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8D 8C 24", "MGS 2 | MGS 3: Custom Resolution: Loading Screen (ENG)"))
                        {
                            static SafetyHookMid MGS2_MGS3_LoadingScreenEngMidHook {};
                            MGS2_MGS3_LoadingScreenEngMidHook = safetyhook::create_mid(MGS2_MGS3_LoadingScreenEngScanResult,
                                                                                       [](SafetyHookContext& ctx)
                                                                                       {
                                                                                           ctx.rdx = iOutputResY >= 2160 ? reinterpret_cast<uintptr_t>(&"$/misc/loading/****/loading_4k.ctxr") :
                                                                                               iOutputResY >= 1440 ? reinterpret_cast<uintptr_t>(&"$/misc/loading/****/loading_wqhd.ctxr") :
                                                                                               /*iOutputResY >= 1080*/ reinterpret_cast<uintptr_t>(&"$/misc/loading/****/loading_fhd.ctxr");
                                                                                       });
                            LOG_HOOK(MGS2_MGS3_LoadingScreenEngMidHook, "MGS 2 | MGS 3: Custom Resolution: Loading Screen (ENG)")
                        }

                        if (uint8_t* MGS2_MGS3_LoadingScreenJPScanResult = Memory::PatternScan(baseModule, "48 8D 4C 24 ?? FF 15 ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8D 4C 24", "MGS 2 | MGS 3: Custom Resolution: Loading Screen (JP)")) //    /loading_jp.ctxr 
                        {
                            static SafetyHookMid MGS2_MGS3_LoadingScreenJPMidHook {};
                            MGS2_MGS3_LoadingScreenJPMidHook = safetyhook::create_mid(MGS2_MGS3_LoadingScreenJPScanResult,
                                                                                      [](SafetyHookContext& ctx)
                                                                                      {
                                                                                          ctx.rdx = iOutputResY >= 2160 ? reinterpret_cast<uintptr_t>(&"$/misc/loading/****/loading_jp_4k.ctxr") :
                                                                                              iOutputResY >= 1440 ? reinterpret_cast<uintptr_t>(&"$/misc/loading/****/loading_jp_wqhd.ctxr") :
                                                                                              /*iOutputResY >= 1080*/ reinterpret_cast<uintptr_t>(&"$/misc/loading/****/loading_jp_fhd.ctxr");
                                                                                      });
                            LOG_HOOK(MGS2_MGS3_LoadingScreenJPMidHook, "MGS 2 | MGS 3: Custom Resolution: Loading Screen (JP)")
                        }
                    }

                }
            }
            else if (!MGS2_MGS3_InternalResolutionScanResult || !MGS2_MGS3_OutputResolution1ScanResult || !MGS2_MGS3_OutputResolution2ScanResult)
            {
                spdlog::error("MG/MG2 | MGS 2 | MGS 3: Custom Resolution: Pattern scan failed.");
            }

            // MG 1/2 | MGS 2 | MGS 3: WindowedMode
            if (uint8_t* MGS2_MGS3_WindowedModeScanResult = Memory::PatternScan(baseModule, "48 ?? ?? E8 ?? ?? ?? ?? 84 ?? 0F 84 ?? ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? 41 ?? 03 00 00 00", "WindowedMode"))
            {
                static SafetyHookMid WindowedModeMidHook {};
                WindowedModeMidHook = safetyhook::create_mid(MGS2_MGS3_WindowedModeScanResult,
                                                             [](SafetyHookContext& ctx)
                                                             {
                                                                 // Force windowed mode if windowed or borderless is set.
                                                                 if (bWindowedMode || bBorderlessMode)
                                                                 {
                                                                     ctx.rdx = 0;
                                                                 }
                                                                 else
                                                                 {
                                                                     ctx.rdx = 1;
                                                                 }
                                                             });
            }


            // MG 1/2 | MGS 2 | MGS 3: CreateWindowExA
            CreateWindowExA_hook = safetyhook::create_inline(CreateWindowExA, reinterpret_cast<void*>(CreateWindowExA_hooked));
            LOG_HOOK(CreateWindowExA_hook, "MG/MG2 | MGS 2 | MGS 3: CreateWindowExA")

            // MG 1/2 | MGS 2 | MGS 3: SetWindowPos
                if (uint8_t* MGS2_MGS3_SetWindowPosScanResult = Memory::PatternScan(baseModule, "33 ?? 48 ?? ?? ?? FF ?? ?? ?? ?? ?? 8B ?? ?? BA 02 00 00 00", "SetWindowPos"))
                {
                    static SafetyHookMid SetWindowPosMidHook {};
                    SetWindowPosMidHook = safetyhook::create_mid(MGS2_MGS3_SetWindowPosScanResult,
                                                                 [](SafetyHookContext& ctx)
                                                                 {
                                                                     if (bBorderlessMode)
                                                                     {
                                                                         // Set X and Y to 0 to position window at centre of screen.
                                                                         ctx.r8 = 0;
                                                                         ctx.r9 = 0;
                                                                         // Set window width and height to desktop resolution.
                                                                         /// todo - add new option - maximize to desktop resolution, or keep at native size
                                                                             /// second option -> single monitor or span multiple monitors
                                                                         //clamp to largest of total screen dimension x or total screen dimension y, then calc ratio for the other dimension to properly clamp. 
                                                                         // that way, window size is either smaller than top/bottom or left/right dimension when clamped to largest screen dimension
                                                                         *reinterpret_cast<int*>(ctx.rsp + 0x20) = (int)DesktopDimensions.first;
                                                                         *reinterpret_cast<int*>(ctx.rsp + 0x28) = (int)DesktopDimensions.second;
                                                                     }
                                                                     else if (bWindowedMode)
                                                                     {
                                                                         const int desktopW = static_cast<int>(DesktopDimensions.first);
                                                                         const int desktopH = static_cast<int>(DesktopDimensions.second);

                                                                         // Actual viewport size, factoring for the frame.
                                                                         const int winW = static_cast<int>(iOutputResX) + 18;
                                                                         const int winH = static_cast<int>(iOutputResY) + 47;

                                                                         // Centering offsets (can be negative)
                                                                         const int offsetX = (desktopW - winW) / 2;
                                                                         const int offsetY = (desktopH - winH) / 2;

                                                                         // X / Y position
                                                                         ctx.r8 = static_cast<uint64_t>(static_cast<int64_t>(offsetX));
                                                                         ctx.r9 = static_cast<uint64_t>(static_cast<int64_t>(offsetY));

                                                                         // Width / Height
                                                                         *reinterpret_cast<int*>(ctx.rsp + 0x20) = winW;
                                                                         *reinterpret_cast<int*>(ctx.rsp + 0x28) = winH;

                                                                     }
                                                                 });

                }


            // MGS 2 | MGS 3: Framebuffer fix, stops the framebuffer from being set to maximum display resolution.
            // Thanks emoose!
            if (bFramebufferFix)
            {
                // Need to stop hor + vert from being modified.
                for (int i = 1; i <= 2; ++i)
                {
                    // Fullscreen framebuffer
                    if (uint8_t* MGS2_MGS3_FullscreenFramebufferFixScanResult = Memory::PatternScanSilent(baseModule, "03 ?? 41 ?? ?? ?? C7 ?? ?? ?? ?? ?? ?? 00 00 00"))
                    {
                        Memory::PatchBytes((uintptr_t)MGS2_MGS3_FullscreenFramebufferFixScanResult + 0x2, "\x90\x90\x90\x90", 4);
                        spdlog::info("MG/MG2 | MGS 2 | MGS 3: Fullscreen Framebuffer {}: Patched instruction.", i);
                    }
                }

                // Windowed framebuffer

                if (uint8_t* MGS2_MGS3_WindowedFramebufferFixScanResult = Memory::PatternScan(baseModule, "?? ?? F3 0F ?? ?? 41 ?? ?? F3 0F ?? ?? F3 0F ?? ?? 66 0F ?? ?? 0F ?? ??", "Windowed Framebuffer"))
                {
                    Memory::PatchBytes((uintptr_t)MGS2_MGS3_WindowedFramebufferFixScanResult, "\xEB", 1);
                    if (eGameType & MG | MGS3)
                    {
                        Memory::PatchBytes((uintptr_t)MGS2_MGS3_WindowedFramebufferFixScanResult + 0x2A, "\xEB", 1);
                    }

                    if (eGameType & MGS2)
                    {
                        Memory::PatchBytes((uintptr_t)MGS2_MGS3_WindowedFramebufferFixScanResult + 0x27, "\xEB", 1);
                    }
                    spdlog::info("MG/MG2 | MGS 2 | MGS 3: Windowed Framebuffer: Patched instructions.");
                }
            }
        }

        /*
        float WidescreenRes = (float)(iOutputResY * 16) / 9;
    
        if (uint8_t* MGS2_MGS3_ViewportScanResult = Memory::PatternScan(baseModule, "48 83 EC ?? 48 8B 05 ?? ?? ?? ?? 4C 8B C2", "MGS 2 | MGS 3: CD3DCachedDevice::SetViewport"))
        {
            static SafetyHookMid Viewport_MidHook {};
            Viewport_MidHook = safetyhook::create_mid(MGS2_MGS3_ViewportScanResult,
                [](SafetyHookContext& ctx)
                {
                    D3D11_VIEWPORT D3DViewport = *reinterpret_cast<D3D11_VIEWPORT*>(ctx.rdx);
                    //check what scales with 1024x1024
                    /*if (D3DViewport.Height == 720.0f)
                    {
                        return;
                    }
    
                    if (D3DViewport.Height == 2160.0f)
                    {
                        return;
                    }
                    if (D3DViewport.Height == 969.0f && (D3DViewport.Width == 645.0f || D3DViewport.Width == 652.0f) && ((D3DViewport.TopLeftX == 2745.0f && D3DViewport.TopLeftY == 106.0f) || (D3DViewport.TopLeftX == 427.0f && D3DViewport.TopLeftY == 106.0f)))
                    {
                        return;
                    }
                    if (D3DViewport.Width == 384.0f && D3DViewport.Height == 288.0f)//videos
                    {
                        return;
                    }
                    spdlog::info("MGS 2 | MGS 3: Viewport: X: {}, Y: {}, Width: {}, Height: {}, Max Depth: {}, Min Depth: {}", D3DViewport.TopLeftX, D3DViewport.TopLeftY, D3DViewport.Width, D3DViewport.Height, D3DViewport.MaxDepth, D3DViewport.MinDepth);
                    /*
                    if (D3DViewport.TopLeftY != 1080.0f)
                    {
                        //D3DViewport.TopLeftX = (float)iOutputResX/2/2/2;
                        D3DViewport.Width = 1920.0f;
                        spdlog::info("MGS 2 | MGS 3: Viewport: New X: {}, Y: {}, Width: {}, Height: {}", D3DViewport.TopLeftX, D3DViewport.TopLeftY, D3DViewport.Width, D3DViewport.Height);
                        *reinterpret_cast<D3D11_VIEWPORT*>(ctx.rdx) = D3DViewport;
    
                    }*//*
                });
        }
        */
    }


    void Init_ScaleEffects()
    {
        if ((eGameType & (MGS2 | MGS3)) && bOutputResolution)
        {
            // MGS 2 | MGS 3: Fix scaling for added volume menu in v1.4.0 patch
            if (uint8_t* MGS2_MGS3_VolumeMenuScanResult = Memory::PatternScan(baseModule, "F3 0F ?? ?? 48 ?? ?? ?? 89 ?? ?? ?? 00 00 F3 0F ?? ?? 89 ?? ?? ?? 00 00", "MGS 2 | MGS 3: Volume Menu"))
            {
                static SafetyHookMid MGS2_MGS3_VolumeMenuMidHook {};
                MGS2_MGS3_VolumeMenuMidHook = safetyhook::create_mid(MGS2_MGS3_VolumeMenuScanResult,
                                                                     [](SafetyHookContext& ctx)
                                                                     {
                                                                         ctx.xmm2.f32[0] = (float)1280;
                                                                         ctx.xmm3.f32[0] = (float)720;
                                                                     });
                LOG_HOOK(MGS2_MGS3_VolumeMenuMidHook, "MGS 2 | MGS 3: Volume Menu")
            }
        }

        if (eGameType & MGS2 && bOutputResolution)
        {
            // MGS 2: Scale Effects
            if (uint8_t* MGS2_ScaleEffectsScanResult = Memory::PatternScan(baseModule, "48 8B ?? ?? 66 ?? ?? ?? 0F ?? ?? F3 0F ?? ?? F3 0F ?? ?? F3 0F ?? ?? ?? ?? ?? ??", "MGS 2: Scale Effects"))
            {
                float fMGS2_DefaultEffectScaleX = *reinterpret_cast<float*>(Memory::GetAbsolute((uintptr_t)MGS2_ScaleEffectsScanResult - 0x4));
                float fMGS2_DefaultEffectScaleY = *reinterpret_cast<float*>(Memory::GetAbsolute((uintptr_t)MGS2_ScaleEffectsScanResult + 0x28));
                spdlog::info("MGS 2: Scale Effects: Default X is {}, Y is {}", fMGS2_DefaultEffectScaleX, fMGS2_DefaultEffectScaleY);

                if (bHUDFix)
                {
                    fMGS2_EffectScaleX = (float)fMGS2_DefaultEffectScaleX / (fDefaultHUDWidth / fHUDWidth);
                    fMGS2_EffectScaleY = (float)fMGS2_DefaultEffectScaleY / (fDefaultHUDHeight / (float)iInternalResY);
                    if (fAspectRatio < fNativeAspect)
                    {
                        fMGS2_EffectScaleX = (float)fMGS2_DefaultEffectScaleX / (fDefaultHUDWidth / (float)iInternalResX);
                        fMGS2_EffectScaleY = (float)fMGS2_DefaultEffectScaleY / (fDefaultHUDWidth / (float)iInternalResX);
                    }
                    spdlog::info("MGS 2: Scale Effects (HUD Fix Enabled): New X is {}, Y is {}", fMGS2_EffectScaleX, fMGS2_EffectScaleY);
                }
                else
                {
                    fMGS2_EffectScaleX = (float)fMGS2_DefaultEffectScaleX / (fDefaultHUDWidth / (float)iInternalResX);
                    fMGS2_EffectScaleY = (float)fMGS2_DefaultEffectScaleY / (fDefaultHUDHeight / (float)iInternalResY);
                    spdlog::info("MGS 2: Scale Effects: New X is {}, Y is {}", fMGS2_EffectScaleX, fMGS2_EffectScaleY);
                }

                static SafetyHookMid ScaleEffectsXMidHook {};
                ScaleEffectsXMidHook = safetyhook::create_mid(MGS2_ScaleEffectsScanResult,
                                                              [](SafetyHookContext& ctx)
                                                              {
                                                                  ctx.xmm1.f32[0] = fMGS2_EffectScaleX;
                                                              });

                static SafetyHookMid ScaleEffectsX2MidHook {};
                ScaleEffectsX2MidHook = safetyhook::create_mid(MGS2_ScaleEffectsScanResult - 0x2B,
                                                               [](SafetyHookContext& ctx)
                                                               {
                                                                   ctx.xmm1.f32[0] = fMGS2_EffectScaleX;
                                                               });

                static SafetyHookMid ScaleEffectsYMidHook {};
                ScaleEffectsYMidHook = safetyhook::create_mid(MGS2_ScaleEffectsScanResult + 0x2C,
                                                              [](SafetyHookContext& ctx)
                                                              {
                                                                  ctx.xmm1.f32[0] = fMGS2_EffectScaleY;
                                                              });
            }
        }

    }




    void Init_AspectFOVFix()
    {
        // Fix aspect ratio
        if (eGameType & MGS3 && bAspectFix)
        {
            // MGS 3: Fix gameplay aspect ratio
            if (uint8_t* MGS3_GameplayAspectScanResult = Memory::PatternScan(baseModule, "F3 0F ?? ?? E8 ?? ?? ?? ?? 48 8D ?? ?? ?? ?? ?? F3 0F ?? ?? ?? ?? ?? ??", "MGS 3: Aspect Ratio"))
            {
                DWORD64 MGS3_GameplayAspectAddress = Memory::GetAbsolute((uintptr_t)MGS3_GameplayAspectScanResult + 0x5);
                spdlog::info("MGS 3: Aspect Ratio: Function address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS3_GameplayAspectAddress - (uintptr_t)baseModule);

                static SafetyHookMid MGS3_GameplayAspectMidHook {};
                MGS3_GameplayAspectMidHook = safetyhook::create_mid(MGS3_GameplayAspectAddress + 0x38,
                                                                    [](SafetyHookContext& ctx)
                                                                    {
                                                                        ctx.xmm1.f32[0] /= fAspectMultiplier;
                                                                    });
                LOG_HOOK(MGS3_GameplayAspectMidHook, "MGS 3: Aspect Ratio")

            }
        }
        else if (eGameType & MGS2 && bAspectFix)
        {
            // MGS 2: Fix gameplay aspect ratio
            if (uint8_t* MGS2_GameplayAspectScanResult = Memory::PatternScan(baseModule, "48 8D ?? ?? ?? E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? F3 44 ?? ?? ?? ?? ?? ?? ??", "MGS 2: Aspect Ratio"))
            {
                DWORD64 MGS2_GameplayAspectAddress = Memory::GetAbsolute((uintptr_t)MGS2_GameplayAspectScanResult + 0xB);
                spdlog::info("MGS 2: Aspect Ratio: Function address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS2_GameplayAspectAddress - (uintptr_t)baseModule);

                static SafetyHookMid MGS2_GameplayAspectMidHook {};
                MGS2_GameplayAspectMidHook = safetyhook::create_mid(MGS2_GameplayAspectAddress + 0x38,
                                                                    [](SafetyHookContext& ctx)
                                                                    {
                                                                        ctx.xmm0.f32[0] /= fAspectMultiplier;
                                                                    });
                LOG_HOOK(MGS2_GameplayAspectMidHook, "MGS 2: Aspect Ratio")
            }
        }

        // Convert FOV to vert- to match 16:9 horizontal field of view
        if (eGameType & MGS3 && bFOVFix)
        {
            // MGS 3: FOV
            if (uint8_t* MGS3_FOVScanResult = Memory::PatternScan(baseModule, "F3 0F ?? ?? ?? ?? ?? ?? 44 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? F3 ?? ?? ?? ?? E8 ?? ?? ?? ??", "MGS 3: FOV"))
            {
                static SafetyHookMid MGS3_FOVMidHook {};
                MGS3_FOVMidHook = safetyhook::create_mid(MGS3_FOVScanResult,
                                                         [](SafetyHookContext& ctx)
                                                         {
                                                             if (fAspectRatio < fNativeAspect)
                                                             {
                                                                 ctx.xmm2.f32[0] *= fAspectMultiplier;
                                                             }
                                                         });
                LOG_HOOK(MGS3_FOVMidHook, "MG3 2: FOV")
            }
        }
        else if (eGameType & MGS2 && bFOVFix)
        {
            // MGS 2: FOV
            if (uint8_t* MGS2_FOVScanResult = Memory::PatternScan(baseModule, "44 ?? ?? ?? ?? F3 0F ?? ?? ?? ?? ?? ?? 44 ?? ?? ?? ?? 48 ?? ?? 48 ?? ?? ?? ?? 00 00", "MGS 2: FOV"))
            {
                static SafetyHookMid MGS2_FOVMidHook {};
                MGS2_FOVMidHook = safetyhook::create_mid(MGS2_FOVScanResult,
                                                         [](SafetyHookContext& ctx)
                                                         {
                                                             if (fAspectRatio < fNativeAspect)
                                                             {
                                                                 ctx.xmm2.f32[0] *= fAspectMultiplier;
                                                             }
                                                         });
                LOG_HOOK(MGS2_FOVMidHook, "MGS 2: FOV")
            }

        }

    }

    void Init_HUDFix()
    {
        if (eGameType & MGS2 && bHUDFix)
        {
            // MGS 2: HUD
            uint8_t* MGS2_HUDWidthScanResult = Memory::PatternScanSilent(baseModule, "E9 ?? ?? ?? ?? F3 0F ?? ?? ?? 0F ?? ?? F3 0F ?? ?? ?? F3 0F ?? ??");
            if (MGS2_HUDWidthScanResult)
            {
                spdlog::info("MGS 2: HUD: Address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS2_HUDWidthScanResult - (uintptr_t)baseModule);

                static SafetyHookMid MGS2_HUDWidthMidHook {};
                MGS2_HUDWidthMidHook = safetyhook::create_mid(MGS2_HUDWidthScanResult + 0xD,
                                                              [](SafetyHookContext& ctx)
                                                              {
                                                                  if (fAspectRatio > fNativeAspect)
                                                                  {
                                                                      ctx.xmm2.f32[0] = 1 / fAspectMultiplier;
                                                                      ctx.xmm9.f32[0] = -2;
                                                                      ctx.xmm10.f32[0] = -1;
                                                                  }
                                                                  else if (fAspectRatio < fNativeAspect)
                                                                  {
                                                                      ctx.xmm2.f32[0] = 1;
                                                                      ctx.xmm9.f32[0] = -2 * fAspectMultiplier;
                                                                      ctx.xmm10.f32[0] = -1 * fAspectMultiplier;
                                                                  }
                                                              });
            }
            else if (!MGS2_HUDWidthScanResult)
            {
                spdlog::error("MGS 2: HUD: Pattern scan failed.");
            }

            // MGS 2: Radar
            uint8_t* MGS2_RadarWidthScanResult = Memory::PatternScanSilent(baseModule, "44 ?? ?? 8B ?? 0F ?? ?? ?? 41 ?? ?? 0F ?? ?? ?? 44 ?? ?? ?? ?? ?? ?? 0F ?? ?? ?? 99");
            if (MGS2_RadarWidthScanResult)
            {
                // Radar width
                DWORD64 MGS2_RadarWidthAddress = (uintptr_t)MGS2_RadarWidthScanResult;
                spdlog::info("MGS 2: Radar Width: Hook address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS2_RadarWidthAddress - (uintptr_t)baseModule);

                static SafetyHookMid MGS2_RadarWidthMidHook {};
                MGS2_RadarWidthMidHook = safetyhook::create_mid(MGS2_RadarWidthScanResult,
                                                                [](SafetyHookContext& ctx)
                                                                {
                                                                    if (fAspectRatio > fNativeAspect)
                                                                    {
                                                                        ctx.rbx = (int)fHUDWidth;
                                                                    }
                                                                    else if (fAspectRatio < fNativeAspect)
                                                                    {
                                                                        ctx.rax = (int)fHUDHeight;
                                                                    }
                                                                });

                // Radar width offset
                DWORD64 MGS2_RadarWidthOffsetAddress = (uintptr_t)MGS2_RadarWidthScanResult + 0x54;
                spdlog::info("MGS 2: Radar Width Offset: Hook address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS2_RadarWidthOffsetAddress - (uintptr_t)baseModule);

                static SafetyHookMid MGS2_RadarWidthOffsetMidHook {};
                MGS2_RadarWidthOffsetMidHook = safetyhook::create_mid(MGS2_RadarWidthOffsetAddress,
                                                                      [](SafetyHookContext& ctx)
                                                                      {
                                                                          ctx.rax += (int)fHUDWidthOffset;
                                                                      });

                // Radar height offset
                DWORD64 MGS2_RadarHeightOffsetAddress = (uintptr_t)MGS2_RadarWidthScanResult + 0x90;
                spdlog::info("MGS 2: Radar Height Offset: Hook address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS2_RadarHeightOffsetAddress - (uintptr_t)baseModule);

                static SafetyHookMid MGS2_RadarHeightOffsetMidHook {};
                MGS2_RadarHeightOffsetMidHook = safetyhook::create_mid(MGS2_RadarHeightOffsetAddress,
                                                                       [](SafetyHookContext& ctx)
                                                                       {
                                                                           ctx.rax = (int)fHUDHeightOffset;
                                                                       });
            }
            else if (!MGS2_RadarWidthScanResult)
            {
                spdlog::error("MGS 2: Radar Fix: Pattern scan failed.");
            }

            // MGS 2: Codec Portraits
            // TODO: Reassess this, it's not right.
            uint8_t* MGS2_CodecPortraitsScanResult = Memory::PatternScanSilent(baseModule, "F3 0F ?? ?? ?? F3 0F ?? ?? F3 0F ?? ?? ?? F3 0F ?? ?? 66 0F ?? ?? 0F ?? ??");
            if (MGS2_CodecPortraitsScanResult)
            {
                spdlog::info("MGS 2: Codec Portraits: Hook address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS2_CodecPortraitsScanResult - (uintptr_t)baseModule);

                static SafetyHookMid MGS2_CodecPortraitsMidHook {};
                MGS2_CodecPortraitsMidHook = safetyhook::create_mid(MGS2_CodecPortraitsScanResult,
                                                                    [](SafetyHookContext& ctx)
                                                                    {
                                                                        if (fAspectRatio > fNativeAspect)
                                                                        {
                                                                            ctx.xmm0.f32[0] /= fAspectMultiplier;
                                                                            ctx.xmm0.f32[0] += (ctx.xmm4.f32[0] / fAspectMultiplier);
                                                                        }
                                                                        else if (fAspectRatio < fNativeAspect)
                                                                        {
                                                                            ctx.xmm5.f32[0] /= fAspectMultiplier;
                                                                        }
                                                                    });
            }
            else if (!MGS2_CodecPortraitsScanResult)
            {
                spdlog::error("MGS 2: Codec Portraits: Pattern scan failed.");
            }

            // MGS 2: Disable motion blur. 
            uint8_t* MGS2_MotionBlurScanResult = Memory::PatternScanSilent(baseModule, "F3 48 ?? ?? ?? ?? 48 ?? ?? ?? 48 ?? ?? ?? F3 0F ?? ?? ?? ?? ?? ?? 0F ?? ??");
            if (MGS2_MotionBlurScanResult)
            {
                spdlog::info("MGS 2: Motion Blur: Address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS2_MotionBlurScanResult - (uintptr_t)baseModule);

                Memory::PatchBytes((uintptr_t)MGS2_MotionBlurScanResult, "\x48\x31\xDB\x90\x90\x90", 6);
                spdlog::info("MGS 2: Motion Blur: Patched instruction.");
            }
            else if (!MGS2_MotionBlurScanResult)
            {
                spdlog::error("MGS 2: Motion Blur: Pattern scan failed.");
            }
        }
        else if (eGameType & MGS3 && bHUDFix || eGameType & MG && fAspectRatio != fNativeAspect)
        {
            // MG1/2 | MGS 3: HUD
            uint8_t* MGS3_HUDWidthScanResult = Memory::PatternScanSilent(baseModule, "0F ?? ?? ?? ?? ?? F3 44 ?? ?? ?? ?? ?? ?? ?? 4C ?? ?? ?? ?? ?? ?? F3 44 ?? ?? ?? ?? ?? ?? ?? 41 ?? 00 02 00 00");
            if (MGS3_HUDWidthScanResult)
            {
                static SafetyHookMid MGS3_HUDWidthMidHook {};
                MGS3_HUDWidthMidHook = safetyhook::create_mid(MGS3_HUDWidthScanResult + 0x1F,
                                                              [](SafetyHookContext& ctx)
                                                              {
                                                                  if (fAspectRatio > fNativeAspect)
                                                                  {
                                                                      ctx.xmm14.f32[0] = 2 / fAspectMultiplier;
                                                                      ctx.xmm15.f32[0] = -2;
                                                                  }
                                                                  else if (fAspectRatio < fNativeAspect)
                                                                  {
                                                                      ctx.xmm14.f32[0] = 2;
                                                                      ctx.xmm15.f32[0] = -2 * fAspectMultiplier;
                                                                  }
                                                              });

                spdlog::info("MG1/2 | MGS 3: HUD Width: Hook address is{:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS3_HUDWidthScanResult - (uintptr_t)baseModule);
            }
            else if (!MGS3_HUDWidthScanResult)
            {
                spdlog::error("MG1/2 | MGS 3: HUD Width: Pattern scan failed.");
            }
        }

        if ((eGameType & (MG | MGS2 | MGS3)) && bHUDFix)
        {
            // MGS 2 | MGS 3: Letterboxing
            uint8_t* MGS2_MGS3_LetterboxingScanResult = Memory::PatternScanSilent(baseModule, "83 ?? 01 75 ?? ?? 01 00 00 00 44 ?? ?? ?? ?? ?? ?? 89 ?? ?? ?? ?? ??");
            if (MGS2_MGS3_LetterboxingScanResult)
            {
                DWORD64 MGS2_MGS3_LetterboxingAddress = (uintptr_t)MGS2_MGS3_LetterboxingScanResult + 0x6;
                spdlog::info("MGS 2 | MGS 3: Letterboxing: Address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)MGS2_MGS3_LetterboxingAddress - (uintptr_t)baseModule);

                Memory::Write(MGS2_MGS3_LetterboxingAddress, (int)0);
                spdlog::info("MGS 2 | MGS 3: Letterboxing: Disabled letterboxing.");
            }
            else if (!MGS2_MGS3_LetterboxingScanResult)
            {
                spdlog::error("MGS 2 | MGS 3: Letterboxing: Pattern scan failed.");
            }
        }

    }
}
