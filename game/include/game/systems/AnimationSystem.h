// Created by Elijah Fabon

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
