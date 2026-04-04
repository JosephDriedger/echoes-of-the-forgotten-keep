// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SWITCHTRIGGERSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SWITCHTRIGGERSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class SwitchTriggerSystem
    {
    public:
        static void Update(engine::Registry& registry);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SWITCHTRIGGERSYSTEM_H
