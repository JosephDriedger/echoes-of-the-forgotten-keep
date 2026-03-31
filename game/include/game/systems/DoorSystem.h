#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DOORSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DOORSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class DoorSystem
    {
    public:
        static void Update(engine::Registry& registry, float deltaTime);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_DOORSYSTEM_H
