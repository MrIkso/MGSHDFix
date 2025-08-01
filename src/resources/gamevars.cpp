#include "common.hpp"
#include "gamevars.hpp"
#include "effect_speeds.hpp"
#include "logging.hpp"
#include "mgs2_sunglasses.hpp"
#include "stat_persistence.hpp"


void GameVars::Initialize()
{
    if (eGameType & MGS2)
    {
        aimingState = reinterpret_cast<uint64_t*>(Memory::GetRelativeOffset(Memory::PatternScan(baseModule, "8B 2D ?? ?? ?? ?? F3 0F 10 1D", "MGS 2: GameVars: aimingState") + 2)); // +17DF86C 2.0.1
        cutsceneFlag = reinterpret_cast<int*>(Memory::GetRelativeOffset(Memory::PatternScan(baseModule, "8B 05 ?? ?? ?? ?? 45 33 F6 8B 0D", "MGS 2: GameVars: cutsceneFlag") + 2));
        scriptedSequenceFlag = reinterpret_cast<int*>(Memory::GetRelativeOffset(Memory::PatternScan(baseModule, "44 39 35 ?? ?? ?? ?? 89 15", "MGS 2: GameVars: scriptedSequenceFlag") + 3));
        actorWaitValue = reinterpret_cast<double*>(Memory::GetRelativeOffset(Memory::PatternScan(baseModule, "66 0F 2F 05 ?? ?? ?? ?? 73 ?? 33 C0", "MGS 2: GameVars: actorWaitValue") + 4));
        currentStage = reinterpret_cast<char const*>(Memory::GetRelativeOffset(Memory::PatternScan(baseModule, "4C 8D 0D ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 4C 8D 05", "MGS 2: GameVars: currentStage") + 3));
        if (g_Logging.bVerboseLogging)
        {
            spdlog::info("GameVars: aimingState address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)aimingState - (uintptr_t)baseModule);
            spdlog::info("GameVars: cutsceneFlag address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)cutsceneFlag - (uintptr_t)baseModule);
            spdlog::info("GameVars: scriptedSequenceFlag address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)scriptedSequenceFlag - (uintptr_t)baseModule);
            spdlog::info("GameVars: actorWaitValue address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)actorWaitValue - (uintptr_t)baseModule);
            spdlog::info("GameVars: currentStage address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)currentStage - (uintptr_t)baseModule);
        }
        if (uint8_t* LevelTransitionResult = Memory::PatternScan(baseModule, "89 73 ?? 81 25", "GameVars: Level Transition"))
        {
            static SafetyHookMid levelTransitionMidHook {};
            levelTransitionMidHook = safetyhook::create_mid(LevelTransitionResult,
                [](SafetyHookContext& ctx)
                {
                    OnLevelTransition();
                });
            LOG_HOOK(levelTransitionMidHook, "GameVars: Level Transition")
        }
    }
    else if (eGameType & MGS3)
    {
        aimingState = reinterpret_cast<uint64_t*>(Memory::GetRelativeOffset(Memory::PatternScan(baseModule, "8B 35 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? 49 89 8D", "MGS 3: GameVars: aimingState") + 2));
        if (g_Logging.bVerboseLogging)
        {
            spdlog::info("GameVars: aimingState address is {:s}+{:X}", sExeName.c_str(), (uintptr_t)aimingState - (uintptr_t)baseModule);
        }
        if (uint8_t* LevelTransitionResult = Memory::PatternScan(baseModule, "89 5F ?? E9 ?? ?? ?? ?? 39 1D", "GameVars: Level Transition"))
        {
            static SafetyHookMid levelTransitionMidHook {};
            levelTransitionMidHook = safetyhook::create_mid(LevelTransitionResult,
                [](SafetyHookContext& ctx)
                {
                    OnLevelTransition();
                });
            LOG_HOOK(levelTransitionMidHook, "GameVars: Level Transition")
        }
    }
}

bool GameVars::InCutscene() const //does not count pad demos or FMVs, only full cutscenes.
{
    return cutsceneFlag == nullptr ? false : (*cutsceneFlag == 1);
}

bool GameVars::InScriptedSequence() const //Scripted sequences, i.e., forced codec calls, cutscenes, pad demos (ingame tutorials & forced movements ala first meeting Stillman), and FMVs.
{
    return scriptedSequenceFlag == nullptr ? false : (*scriptedSequenceFlag == 1);
}

double GameVars::ActorWaitMultiplier() const
{
    return actorWaitValue == nullptr ? 1.0 : ((1.0/60) / *actorWaitValue);
}

const char* GameVars::GetCurrentStage() const
{
    return currentStage == nullptr ? "nullptr" : currentStage;
}


void GameVars::SetAimingState(const uint64_t state) const
{
    if (aimingState != nullptr)
    {
        *aimingState = state;
    }
    else
    {
        spdlog::error("GameVars: SetAimingState: aimingState is null!");
    }
}

uint64_t GameVars::GetAimingState() const
{
    if (aimingState != nullptr)
    {
        return *aimingState;
    }
    else
    {
        spdlog::error("GameVars: GetAimingState: aimingState is null!");
        return 0;
    }
}

void GameVars::OnLevelTransition()
{
    g_EffectSpeedFix.Reset();
    g_StatPersistence.SaveStats();
    MGS2Sunglasses::CheckOnTransition();
}

