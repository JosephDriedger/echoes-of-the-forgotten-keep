// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SYSTEM_H

#include <set>

#include "engine/ecs/Entity.h"

namespace engine
{
    class System
    {
    public:
        virtual ~System() = default;

        const std::set<Entity>& GetEntities() const;

    protected:
        std::set<Entity> m_Entities;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_SYSTEM_H