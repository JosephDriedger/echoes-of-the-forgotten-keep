// Created by Elijah Fabon

// AnimationSystem -- Drives skeletal animation for all entities.
// Manages a per-entity animation state machine (Idle, Run, Attack1-3,
// HitReact, Death), handles clip blending/transitions, advances combo
// attack timers, and computes final bone transforms each frame.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/resources/AudioEventQueue.h"

namespace game
{
    class AnimationSystem
    {
    public:
        static void Update(engine::Registry& registry, float deltaTime, engine::AudioEventQueue& audioEventQueue);
    };
}

#endif
