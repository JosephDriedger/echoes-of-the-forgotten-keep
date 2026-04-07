// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SYSTEM_H

#include <set>

#include "engine/ecs/Entity.h"

namespace engine
{
    // Base class for all ECS systems. A system operates on the set of entities
    // whose signatures match its required components. Derived classes implement
    // game logic (e.g. rendering, physics) by iterating over m_Entities.
    //
    // The entity set is managed externally (typically by a SystemManager or
    // similar coordinator that updates m_Entities when entity signatures change).
    class System
    {
    public:
        virtual ~System() = default;

        const std::set<Entity>& GetEntities() const;

    protected:
        // Entities that match this system's required component signature.
        std::set<Entity> m_Entities;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_SYSTEM_H