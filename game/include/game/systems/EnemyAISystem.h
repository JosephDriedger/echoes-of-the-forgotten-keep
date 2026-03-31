#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAISYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAISYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

namespace game
{
    class EnemyAISystem
    {
    public:
        void Update(engine::Registry& registry, float deltaTime);

    private:
        void UpdateIdle(engine::Registry& registry, engine::Entity enemy, float dt);
        void UpdateChase(engine::Registry& registry, engine::Entity enemy,
                         engine::Entity player, float dt);
        void UpdateAttack(engine::Registry& registry, engine::Entity enemy,
                          engine::Entity player, float dt);

        static float Distance(float ax, float az, float bx, float bz);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ENEMYAISYSTEM_H
