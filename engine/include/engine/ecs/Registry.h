//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H

#include <array>
#include <cstddef>
#include <queue>
#include <vector>

#include "engine/ecs/Entity.h"
#include "engine/ecs/Signature.h"

namespace engine
{
    class Registry
    {
    public:
        Registry();

        Entity CreateEntity();
        void DestroyEntity(Entity entity);

        bool IsAlive(Entity entity) const;

        void Reset();

        std::size_t GetAliveCount() const;
        std::size_t GetAvailableCount() const;

        void SetSignature(Entity entity, const Signature& signature);
        const Signature& GetSignature(Entity entity) const;
        void ClearSignature(Entity entity);

        const std::vector<Entity>& GetActiveEntities() const;

    private:
        bool IsInRange(EntityId entityId) const;
        void RebuildActiveEntityList();

    private:
        std::queue<EntityId> m_AvailableEntityIds;
        std::array<bool, MAX_ENTITIES + 1> m_Alive;
        std::array<Signature, MAX_ENTITIES + 1> m_Signatures;
        std::vector<Entity> m_ActiveEntities;
        std::size_t m_AliveEntityCount;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H