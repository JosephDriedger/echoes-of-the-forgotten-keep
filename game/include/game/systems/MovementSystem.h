// Created by Elijah Fabon

// MovementSystem -- Translates WASD/arrow key input into player movement
// and rotation. Blocks movement during attack animations and incoming hits.
// Also handles the forward lunge during melee attacks.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MOVEMENTSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MOVEMENTSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/input/Input.h"

namespace game
{
    class MovementSystem
    {
    public:
        static void Update(engine::Registry& registry, const engine::Input& input, float deltaTime);
    };
}

#endif
