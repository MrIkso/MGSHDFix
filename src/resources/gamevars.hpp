#pragma once

class GameVars final
{
private:
    static void OnLevelTransition();

    uint64_t* aimingState = nullptr;
    int* cutsceneFlag = nullptr;
    int* scriptedSequenceFlag = nullptr;
    double* actorWaitValue = nullptr;
    const char* currentStage = nullptr;

public:
    void Initialize();
    bool InCutscene() const; // If we're in a full demo cutscene.
    bool InScriptedSequence() const; // If the game is in a scripted sequence (cutscene or pad demo).
    double ActorWaitMultiplier() const;
    const char* GetCurrentStage() const;

    void SetAimingState(uint64_t state) const;
    uint64_t GetAimingState() const;
};

inline GameVars g_GameVars;
