// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITYSPAWNSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITYSPAWNSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class EntitySpawnSystem
    {
    public:
        static void RegisterComponents(engine::Registry& registry);
        static void Register(engine::Registry& registry);

        static engine::Entity SpawnPlayer(engine::Registry& registry);
        static engine::Entity SpawnEnemy(engine::Registry& registry, float x, float y);
        static engine::Entity SpawnExit(engine::Registry& registry, float x, float y);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITYSPAWNSYSTEM_H