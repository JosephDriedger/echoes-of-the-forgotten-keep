// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ROOMTRANSITIONSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ROOMTRANSITIONSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/scene/RoomManager.h"

namespace game
{
    class RoomTransitionSystem
    {
    public:
        void Update(engine::Registry& registry,
                    engine::RoomManager& roomManager) const;
    };
}

#endif