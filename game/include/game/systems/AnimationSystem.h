#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class AnimationSystem
    {
    public:
        static void Update(engine::Registry& registry, float deltaTime);
    };
}

#endif
