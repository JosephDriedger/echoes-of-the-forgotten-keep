#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/ecs/Entity.h"

namespace game
{
    class AttackHitboxSystem
    {
    public:
        void Update(engine::Registry& registry);

    private:
        engine::Entity m_AttackHitbox{0};
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKHITBOXSYSTEM_H
