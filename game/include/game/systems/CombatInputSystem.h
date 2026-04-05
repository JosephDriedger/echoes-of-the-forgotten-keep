// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATINPUTSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATINPUTSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/input/Input.h"
#include "engine/resources/AudioEventQueue.h"

namespace game
{
    class CombatInputSystem
    {
    public:
        static void Update(engine::Registry& registry, const engine::Input& input, engine::AudioEventQueue& audioEventQueue);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATINPUTSYSTEM_H
