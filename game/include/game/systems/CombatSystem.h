//
// Created by scept on 2026-03-11.
//

#ifndef INC_8051TUTORIAL_COMBATSYSTEM_H
#define INC_8051TUTORIAL_COMBATSYSTEM_H

#pragma once

#include "engine/ecs/Registry.h"
#include "glm/vec3.hpp"

namespace game
{
    class CombatSystem
    {
    public:
        static void Update(engine::Registry& registry, float dt);

    private:
        static void SpawnProjectile(engine::Registry& registry,
                                    engine::Entity attacker,
                                    const glm::vec3& direction,
                                    const std::string& tag);
    };
}

#endif //INC_8051TUTORIAL_COMBATSYSTEM_H