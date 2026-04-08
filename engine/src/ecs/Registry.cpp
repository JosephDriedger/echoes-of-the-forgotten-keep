// Created by Joey Driedger

#include "engine/ecs/Registry.h"

#include <limits>
#include <stdexcept>

namespace engine
{
    namespace
    {
        // Sentinel value for m_ActiveEntityIndices, indicating the entity is not
        // in the active list (either never created or already destroyed).
        constexpr std::size_t INVALID_ACTIVE_ENTITY_INDEX = std::numeric_limits<std::size_t>::max();
    }

    Registry::Registry()
        : m_AvailableEntityIds(),
          m_Alive{},
          m_Signatures{},
          m_ActiveEntityIndices{},
          m_ActiveEntities{},
          m_AliveEntityCount(0),
          m_ComponentTypes{},
          m_ComponentStorages{},
          m_NextComponentType(0)
    {
        Reset();
    }

    Entity Registry::CreateEntity()
    {
        if (m_AvailableEntityIds.empty())
        {
            throw std::runtime_error("Registry::CreateEntity failed: maximum entity count reached.");
        }

        const EntityId entityId = m_AvailableEntityIds.top();
        m_AvailableEntityIds.pop();

        m_Alive[entityId] = true;
        m_Signatures[entityId].reset();
        m_ActiveEntityIndices[entityId] = m_ActiveEntities.size();
        m_ActiveEntities.emplace_back(entityId);
        ++m_AliveEntityCount;

        return Entity(entityId);
    }

    // Destroys an entity: strips all its components, removes it from the active
    // list via swap-and-pop, resets its state, and returns its ID to the pool.
    void Registry::DestroyEntity(const Entity entity)
    {
        const EntityId entityId = entity.GetId();

        if (!IsAlive(entity))
        {
            return;
        }

        RemoveAllComponentsFromEntity(entity);

        const std::size_t removedIndex = m_ActiveEntityIndices[entityId];
        const std::size_t lastIndex = m_ActiveEntities.size() - 1;

        if (removedIndex != lastIndex)
        {
            const Entity movedEntity = m_ActiveEntities[lastIndex];

            m_ActiveEntities[removedIndex] = movedEntity;
            m_ActiveEntityIndices[movedEntity.GetId()] = removedIndex;
        }

        m_ActiveEntities.pop_back();

        ResetEntityState(entityId);
        m_AvailableEntityIds.push(entityId);
        --m_AliveEntityCount;
    }

    // Copies the active list before iterating because DestroyEntity modifies it.
    void Registry::DestroyAllEntities()
    {
        const std::vector<Entity> activeEntities = m_ActiveEntities;

        for (const Entity entity : activeEntities)
        {
            DestroyEntity(entity);
        }
    }

    bool Registry::IsAlive(const Entity entity) const
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
        m_ActiveEntityIndices.fill(INVALID_ACTIVE_ENTITY_INDEX);

        for (Signature& signature : m_Signatures)
        {
            signature.reset();
        }

        for (const std::shared_ptr<IComponentStorage>& storage : m_ComponentStorages)
        {
            if (storage)
            {
                storage->Clear();
            }
        }

        // Push IDs in descending order so the stack pops them in ascending order,
        // meaning the first entity created gets the lowest available ID.
        for (EntityId entityId = MAX_ENTITIES; entityId >= MIN_ENTITY_ID; --entityId)
        {
            m_AvailableEntityIds.push(entityId);
        }

        m_ActiveEntities.clear();
        m_AliveEntityCount = 0;

        ClearComponentRegistrations();
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

    void Registry::ClearComponentRegistrations()
    {
        m_ComponentTypes.clear();
        m_ComponentStorages.fill(nullptr);
        m_NextComponentType = 0;
    }

    std::size_t Registry::GetRegisteredComponentCount() const
    {
        return m_ComponentTypes.size();
    }

    bool Registry::IsInRange(const EntityId entityId)
    {
        return entityId >= MIN_ENTITY_ID && entityId <= MAX_ENTITIES;
    }

    void Registry::RemoveAllComponentsFromEntity(const Entity entity) const
    {
        for (const std::shared_ptr<IComponentStorage>& storage : m_ComponentStorages)
        {
            if (storage)
            {
                storage->Remove(entity);
            }
        }
    }

    void Registry::ResetEntityState(const EntityId entityId)
    {
        m_Alive[entityId] = false;
        m_Signatures[entityId].reset();
        m_ActiveEntityIndices[entityId] = INVALID_ACTIVE_ENTITY_INDEX;
    }
}