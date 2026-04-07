// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATINPUTSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATINPUTSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/input/Input.h"
#include "engine/resources/AudioEventQueue.h"

namespace game
{
    /// Reads player input (spacebar) and drives the 3-hit combo attack system.
    /// On first press: starts attack with ComboIndex 0 and plays the attack animation.
    /// During an active attack: if pressed after 30% animation progress, queues the
    /// next combo hit so the animation system can chain it seamlessly.
    class CombatInputSystem
    {
    public:
        static void Update(engine::Registry& registry, const engine::Input& input, engine::AudioEventQueue& audioEventQueue);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_COMBATINPUTSYSTEM_H
