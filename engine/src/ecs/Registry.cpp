//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/ecs/Registry.h"

#include <stdexcept>

namespace engine
{
    Registry::Registry()
        : m_Alive{},
          m_Signatures{},
          m_ActiveEntities{},
          m_AliveEntityCount(0)
    {
        Reset();
    }

    Entity Registry::CreateEntity()
    {
        if (m_AvailableEntityIds.empty())
        {
            throw std::runtime_error("Registry::CreateEntity failed: maximum entity count reached.");
        }

        const EntityId entityId = m_AvailableEntityIds.front();
        m_AvailableEntityIds.pop();

        m_Alive[entityId] = true;
        m_Signatures[entityId].reset();
        ++m_AliveEntityCount;

        const Entity entity(entityId);
        m_ActiveEntities.push_back(entity);

        return entity;
    }

    void Registry::DestroyEntity(Entity entity)
    {
        const EntityId entityId = entity.GetId();

        if (!IsAlive(entity))
        {
            return;
        }

        m_Alive[entityId] = false;
        m_Signatures[entityId].reset();
        m_AvailableEntityIds.push(entityId);
        --m_AliveEntityCount;

        RebuildActiveEntityList();
    }

    bool Registry::IsAlive(Entity entity) const
    {
        const EntityId entityId = entity.GetId();

        if (!IsInRange(entityId))
        {
            return false;
        }

        return m_Alive[entityId];
    }

    void Registry::Reset()
    {
        while (!m_AvailableEntityIds.empty())
        {
            m_AvailableEntityIds.pop();
        }

        m_Alive.fill(false);

        for (Signature& signature : m_Signatures)
        {
            signature.reset();
        }

        for (EntityId entityId = MIN_ENTITY_ID; entityId <= MAX_ENTITIES; ++entityId)
        {
            m_AvailableEntityIds.push(entityId);
        }

        m_ActiveEntities.clear();
        m_AliveEntityCount = 0;
    }

    std::size_t Registry::GetAliveCount() const
    {
        return m_AliveEntityCount;
    }

    std::size_t Registry::GetAvailableCount() const
    {
        return m_AvailableEntityIds.size();
    }

    void Registry::SetSignature(Entity entity, const Signature& signature)
    {
        if (!IsAlive(entity))
        {
            throw std::runtime_error("Registry::SetSignature failed: entity is not alive.");
        }

        m_Signatures[entity.GetId()] = signature;
    }

    const Signature& Registry::GetSignature(Entity entity) const
    {
        if (!IsAlive(entity))
        {
            throw std::runtime_error("Registry::GetSignature failed: entity is not alive.");
        }

        return m_Signatures[entity.GetId()];
    }

    void Registry::ClearSignature(Entity entity)
    {
        if (!IsAlive(entity))
        {
            return;
        }

        m_Signatures[entity.GetId()].reset();
    }

    const std::vector<Entity>& Registry::GetActiveEntities() const
    {
        return m_ActiveEntities;
    }

    bool Registry::IsInRange(const EntityId entityId) const
    {
        return entityId >= MIN_ENTITY_ID && entityId <= MAX_ENTITIES;
    }

    void Registry::RebuildActiveEntityList()
    {
        m_ActiveEntities.clear();
        m_ActiveEntities.reserve(m_AliveEntityCount);

        for (EntityId entityId = MIN_ENTITY_ID; entityId <= MAX_ENTITIES; ++entityId)
        {
            if (m_Alive[entityId])
            {
                m_ActiveEntities.emplace_back(entityId);
            }
        }
    }
}