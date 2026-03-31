//
// Created by scept on 2026-03-09.
//

#ifndef INC_8051TUTORIAL_ENEMYAISYSTEM_H
#define INC_8051TUTORIAL_ENEMYAISYSTEM_H

#pragma once

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"
#include "game/components/Transform.h"
#include "game/components/AI.h"
#include "game/components/Player.h"
#include "game/components/AnimationState.h"

namespace game
{
    class EnemyAISystem
    {
    public:
        static void Update(engine::Registry& registry, float dt);

    private:
        static engine::Entity FindPlayer(engine::Registry& registry);
        static float Distance(const Transform& a, const Transform& b);
        static glm::vec3 Normalize(const glm::vec3& v);
    };
}

#endif //INC_8051TUTORIAL_ENEMYAISYSTEM_H