#include "pause_on_focus_loss.hpp"

#include "common.hpp"
#include "gamevars.hpp"
#include "helper.hpp"
#include <spdlog/spdlog.h>


bool PauseOnFocusLoss::ShouldFixPauseState()
{
    return g_PauseOnFocusLoss.bPauseOnFocusLoss ? (g_GameVars.InCutscene() || g_GameVars.InPadDemo()) : true;
}

void PauseOnFocusLoss::Initialize()
{
    if (!(eGameType & (MG|MGS2|MGS3)))
    {
        return;
    }

    if (uint8_t* NHT_COsContext_SetShouldPauseApplicationResult = Memory::PatternScan(baseModule, "44 8B 2D ?? ?? ?? ?? 46 89 B4 A6", "BP_COsContext_ShouldPauseApplication", NULL, NULL))
    {
        static SafetyHookMid NHT_COsContext_SetShouldPauseApplicationMidHook {};
        NHT_COsContext_SetShouldPauseApplicationMidHook = safetyhook::create_mid(NHT_COsContext_SetShouldPauseApplicationResult,
            [](SafetyHookContext& ctx)
            {
                if (g_PauseOnFocusLoss.ShouldFixPauseState())
                {
                    ctx.rax = 0;
                }
            });
        LOG_HOOK(NHT_COsContext_SetShouldPauseApplicationMidHook, "Alt-Tab Fix: BP_COsContext_ShouldPauseApplication", NULL, NULL)
    }


    if (uint8_t* BP_COsContext_ShouldPauseApplication_SomeGlobalPlace_Result = Memory::PatternScan(baseModule, "85 C0 74 ?? F7 05 ?? ?? ?? ?? ?? ?? ?? ?? 75", "BP_COsContext_ShouldPauseApplication_SomeGlobalPlace", NULL, NULL))
    {
        static SafetyHookMid BP_COsContext_ShouldPauseApplication_SomeGlobalPlaceMidHook {};
        BP_COsContext_ShouldPauseApplication_SomeGlobalPlaceMidHook = safetyhook::create_mid(BP_COsContext_ShouldPauseApplication_SomeGlobalPlace_Result,
            [](SafetyHookContext& ctx)
            {
                if (g_PauseOnFocusLoss.ShouldFixPauseState())
                {
                    ctx.rax = 0;
                }
            });
        LOG_HOOK(BP_COsContext_ShouldPauseApplication_SomeGlobalPlaceMidHook, "Alt-Tab Fix: BP_COsContext_ShouldPauseApplication_SomeGlobalPlace", NULL, NULL)
    }

    if (uint8_t* BP_COsContext_ShouldPauseApplication_InputsNProcess_Result = Memory::PatternScan(baseModule, "85 C0 74 ?? 48 83 C6", "BP_COsContext_ShouldPauseApplication_InputsAndProcessing", NULL, NULL))
    {
        static SafetyHookMid BP_COsContext_ShouldPauseApplication_InputsNProcessMidHook {};
        BP_COsContext_ShouldPauseApplication_InputsNProcessMidHook = safetyhook::create_mid(BP_COsContext_ShouldPauseApplication_InputsNProcess_Result,
            [](SafetyHookContext& ctx)
            {
                if (g_PauseOnFocusLoss.ShouldFixPauseState())
                {
                    ctx.rax = 0;
                }
            });
        LOG_HOOK(BP_COsContext_ShouldPauseApplication_InputsNProcessMidHook, "Alt-Tab Fix: BP_COsContext_ShouldPauseApplication_InputsNProcess", NULL, NULL)
    }

    
    if (uint8_t* NHT_GetIsMinimizedResult = Memory::PatternScan(baseModule, "48 85 C0 75 ?? C3 83 B8", "NHT_GetIsMinimized", NULL, NULL))
    {
        static SafetyHookMid NHT_GetIsMinimizedMidHook {};
        NHT_GetIsMinimizedMidHook = safetyhook::create_mid(NHT_GetIsMinimizedResult,
            [](SafetyHookContext& ctx)
            {
                if (g_PauseOnFocusLoss.ShouldFixPauseState())
                {
                    ctx.rax = 0;
                }
            });
        LOG_HOOK(NHT_GetIsMinimizedMidHook, "Alt-Tab Fix: NHT_GetIsMinimized", NULL, NULL)
    }

    /****************************************************************
     *                          Key States
     *  Contained in ProcessInputsAndHandlePausing / 40 56 41 57 48 83 EC
     *  Alt key (18)
     *  Left Windows (91)
     *  Right Windows (92)
     *  F10 (121)
     ****************************************************************/

    std::string AltKeyPattern = eGameType & MG ? "66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? B9" :
                      /*eGameType & (MGS2|MGS3)*/"66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? B9";
    uint8_t* GetAsyncKeyStateTestResult = Memory::PatternScan(baseModule, AltKeyPattern.c_str(), "Alt-Tab Fix: Alt Key", NULL, NULL);
    if (GetAsyncKeyStateTestResult)
    {
        static SafetyHookMid KeyStateAlt_MidHook{};
        KeyStateAlt_MidHook = safetyhook::create_mid(GetAsyncKeyStateTestResult,
            [](SafetyHookContext& ctx)
            {
                if (g_PauseOnFocusLoss.ShouldFixPauseState())
                {
                    ctx.rax = 0;
                }
            });
        LOG_HOOK(KeyStateAlt_MidHook, "Alt-Tab Fix: Alt Key", NULL, NULL)
    }


    std::string LeftWindowsKeyPattern = eGameType & MG ? "66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? E8" :
                                      eGameType & MGS2 ? "66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? 48 89 5C 24":
                                    /*eGameType & MGS3*/ "66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? E8";
    GetAsyncKeyStateTestResult = Memory::PatternScan(baseModule, LeftWindowsKeyPattern.c_str(), "Alt-Tab Fix: Left Windows Key", NULL, NULL);
    if (GetAsyncKeyStateTestResult)
    {
        static SafetyHookMid KeyStateLeftWin_Hook{};
        KeyStateLeftWin_Hook = safetyhook::create_mid(GetAsyncKeyStateTestResult,
            [](SafetyHookContext& ctx)
            {
                if (g_PauseOnFocusLoss.ShouldFixPauseState())
                {
                    ctx.rax = 0;
                }
            });
        LOG_HOOK(KeyStateLeftWin_Hook, "Alt-Tab Fix: Left Windows Key", NULL, NULL)
    }

    std::string RightWindowsKeyPattern = eGameType & MG ? "66 85 C0 0F 85 ?? ?? ?? ?? E8" :
                                       eGameType & MGS2 ? "66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? 48 89 5C 24" :
                                     /*eGameType & MGS3*/ "66 85 C0 0F 85 ?? ?? ?? ?? B9 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 66 85 C0 0F 85 ?? ?? ?? ?? E8";
    GetAsyncKeyStateTestResult = Memory::PatternScan(baseModule, RightWindowsKeyPattern.c_str(), "Alt-Tab Fix: Right Windows Key", NULL, NULL);
    if (GetAsyncKeyStateTestResult)
    {
        static SafetyHookMid KeyStateRightWin_Hook{};
        KeyStateRightWin_Hook = safetyhook::create_mid(GetAsyncKeyStateTestResult,
            [](SafetyHookContext& ctx)
            {
                if (g_PauseOnFocusLoss.ShouldFixPauseState())
                {
                    ctx.rax = 0;
                }
            });
        LOG_HOOK(KeyStateRightWin_Hook, "Alt-Tab Fix: Right Windows Key", NULL, NULL)
    }

    if (eGameType & (MGS2|MGS3))
    {
        std::string F10KeyPattern = eGameType & MGS2 ? "66 85 C0 0F 85 ?? ?? ?? ?? 48 89 5C 24" :
                                  /*eGameType & MGS3*/ "66 85 C0 0F 85 ?? ?? ?? ?? E8";
            GetAsyncKeyStateTestResult = Memory::PatternScan(baseModule, F10KeyPattern.c_str(), "Alt-Tab Fix: F10 Key", NULL, NULL);
        if (GetAsyncKeyStateTestResult)
        {
            static SafetyHookMid KeyStateF10_Hook {};
            KeyStateF10_Hook = safetyhook::create_mid(GetAsyncKeyStateTestResult,
                [](SafetyHookContext& ctx)
                {
                    if (g_PauseOnFocusLoss.ShouldFixPauseState())
                    {
                        ctx.rax = 0;
                    }
                });
            LOG_HOOK(KeyStateF10_Hook, "Alt-Tab Fix: F10 Key", NULL, NULL)
        }
    }



}