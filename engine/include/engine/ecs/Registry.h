// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H

#include <array>
#include <cstddef>
#include <memory>
#include <stack>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "engine/ecs/Component.h"
#include "engine/ecs/ComponentStorage.h"
#include "engine/ecs/Entity.h"
#include "engine/ecs/Signature.h"

namespace engine
{
    // Central ECS coordinator that owns all entities, component storage, and signatures.
    //
    // Entity management uses a recycling pool: destroyed entity IDs are pushed onto
    // a stack and reused by future CreateEntity() calls. Active entities are tracked
    // in a dense vector (m_ActiveEntities) with swap-and-pop removal for O(1) destroy.
    //
    // Component registration maps C++ types to sequential ComponentType IDs at runtime
    // via std::type_index, then stores components in type-specific packed arrays
    // (ComponentStorage<T>) accessed through the type-erased IComponentStorage interface.
    //
    // Each entity's signature bitset is updated automatically when components are
    // added or removed, enabling systems to efficiently query matching entities.
    class Registry
    {
    public:
        Registry();

        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        void DestroyAllEntities();

        [[nodiscard]] bool IsAlive(Entity entity) const;

        // Fully resets the registry: destroys all entities, clears all component
        // storage, and re-populates the available entity ID pool.
        void Reset();

        [[nodiscard]] std::size_t GetAliveCount() const;
        [[nodiscard]] std::size_t GetAvailableCount() const;

        void SetSignature(Entity entity, const Signature& signature);
        [[nodiscard]] const Signature& GetSignature(Entity entity) const;
        void ClearSignature(Entity entity);

        [[nodiscard]] const std::vector<Entity>& GetActiveEntities() const;

        // Assigns a new ComponentType ID to TComponent and creates its storage.
        // Safe to call multiple times; subsequent calls for the same type are no-ops.
        template <typename TComponent>
        void RegisterComponent();

        // Returns the ComponentType ID previously assigned to TComponent.
        template <typename TComponent>
        [[nodiscard]] ComponentType GetComponentType() const;

        template <typename TComponent>
        [[nodiscard]] bool IsComponentRegistered() const;

        template <typename TComponent>
        void AddComponent(Entity entity, const TComponent& component);

        template <typename TComponent, typename... TArgs>
        TComponent& EmplaceComponent(Entity entity, TArgs&&... args);

        template <typename TComponent>
        void RemoveComponent(Entity entity);

        template <typename TComponent>
        [[nodiscard]] bool HasComponent(Entity entity) const;

        template <typename TComponent>
        TComponent& GetComponent(Entity entity);

        template <typename TComponent>
        const TComponent& GetComponent(Entity entity) const;

        void ClearComponentRegistrations();
        [[nodiscard]] std::size_t GetRegisteredComponentCount() const;

    private:
        static bool IsInRange(EntityId entityId) ;
        void RemoveAllComponentsFromEntity(Entity entity) const;
        void ResetEntityState(EntityId entityId);

        // Downcasts the type-erased IComponentStorage to the concrete typed storage.
        template <typename TComponent>
        ComponentStorage<TComponent>& GetComponentStorage();

        template <typename TComponent>
        const ComponentStorage<TComponent>& GetComponentStorage() const;

        std::stack<EntityId> m_AvailableEntityIds;                      // Recycled IDs ready for reuse.
        std::array<bool, MAX_ENTITIES + 1> m_Alive;                     // Indexed by EntityId; true if entity is alive.
        std::array<Signature, MAX_ENTITIES + 1> m_Signatures;           // Indexed by EntityId; tracks owned components.
        std::array<std::size_t, MAX_ENTITIES + 1> m_ActiveEntityIndices; // EntityId -> index in m_ActiveEntities (for O(1) removal).
        std::vector<Entity> m_ActiveEntities;                           // Dense list of all currently alive entities.
        std::size_t m_AliveEntityCount;

        std::unordered_map<std::type_index, ComponentType> m_ComponentTypes; // C++ type -> ComponentType ID mapping.
        std::array<std::shared_ptr<IComponentStorage>, MAX_COMPONENTS> m_ComponentStorages; // Indexed by ComponentType.
        ComponentType m_NextComponentType;                              // Next ID to assign on RegisterComponent().
    };

    template <typename TComponent>
    void Registry::RegisterComponent()
    {
        const std::type_index typeIndex(typeid(TComponent));

        if (m_ComponentTypes.contains(typeIndex))
        {
            return;
        }

        if (m_NextComponentType >= MAX_COMPONENTS)
        {
            throw std::runtime_error("Registry::RegisterComponent failed: maximum component count reached.");
        }

        const ComponentType componentType = m_NextComponentType;

        m_ComponentTypes.emplace(typeIndex, componentType);
        m_ComponentStorages[componentType] = std::make_shared<ComponentStorage<TComponent>>();
        ++m_NextComponentType;
    }

    template <typename TComponent>
    ComponentType Registry::GetComponentType() const
    {
        const std::type_index typeIndex(typeid(TComponent));
        const auto iterator = m_ComponentTypes.find(typeIndex);

        if (iterator == m_ComponentTypes.end())
        {
            throw std::runtime_error("Registry::GetComponentType failed: component type is not registered.");
        }

        return iterator->second;
    }

    template <typename TComponent>
    bool Registry::IsComponentRegistered() const
    {
        const std::type_index typeIndex(typeid(TComponent));
        return m_ComponentTypes.contains(typeIndex);
    }

    template <typename TComponent>
    ComponentStorage<TComponent>& Registry::GetComponentStorage()
    {
        const ComponentType componentType = GetComponentType<TComponent>();
        const std::shared_ptr<IComponentStorage>& storage = m_ComponentStorages[componentType];

        if (!storage)
        {
            throw std::runtime_error("Registry::GetComponentStorage failed: storage does not exist for registered component.");
        }

        return static_cast<ComponentStorage<TComponent>&>(*storage);
    }

    template <typename TComponent>
    const ComponentStorage<TComponent>& Registry::GetComponentStorage() const
    {
        const ComponentType componentType = GetComponentType<TComponent>();
        const std::shared_ptr<IComponentStorage>& storage = m_ComponentStorages[componentType];

        if (!storage)
        {
            throw std::runtime_error("Registry::GetComponentStorage failed: storage does not exist for registered component.");
        }

        return static_cast<const ComponentStorage<TComponent>&>(*storage);
    }

    // Adds a component and sets the corresponding bit in the entity's signature.
    template <typename TComponent>
    void Registry::AddComponent(Entity entity, const TComponent& component)
    {
        if (!IsAlive(entity))
        {
            throw std::runtime_error("Registry::AddComponent failed: entity is not alive.");
        }

        ComponentStorage<TComponent>& storage = GetComponentStorage<TComponent>();
        storage.Add(entity, component);

        m_Signatures[entity.GetId()].set(GetComponentType<TComponent>());
    }

    template <typename TComponent, typename... TArgs>
    TComponent& Registry::EmplaceComponent(Entity entity, TArgs&&... args)
    {
        if (!IsAlive(entity))
        {
            throw std::runtime_error("Registry::EmplaceComponent failed: entity is not alive.");
        }

        ComponentStorage<TComponent>& storage = GetComponentStorage<TComponent>();
        TComponent& component = storage.Emplace(entity, std::forward<TArgs>(args)...);

        m_Signatures[entity.GetId()].set(GetComponentType<TComponent>());
        return component;
    }

    // Removes the component and clears the corresponding bit in the entity's signature.
    template <typename TComponent>
    void Registry::RemoveComponent(Entity entity)
    {
        if (!IsAlive(entity))
        {
            return;
        }

        ComponentStorage<TComponent>& storage = GetComponentStorage<TComponent>();
        storage.Remove(entity);

        m_Signatures[entity.GetId()].reset(GetComponentType<TComponent>());
    }

    template <typename TComponent>
    bool Registry::HasComponent(Entity entity) const
    {
        if (!IsAlive(entity))
        {
            return false;
        }

        const ComponentStorage<TComponent>& storage = GetComponentStorage<TComponent>();
        return storage.Has(entity);
    }

    template <typename TComponent>
    TComponent& Registry::GetComponent(Entity entity)
    {
        if (!IsAlive(entity))
        {
            throw std::runtime_error("Registry::GetComponent failed: entity is not alive.");
        }

        ComponentStorage<TComponent>& storage = GetComponentStorage<TComponent>();
        return storage.Get(entity);
    }

    template <typename TComponent>
    const TComponent& Registry::GetComponent(Entity entity) const
    {
        if (!IsAlive(entity))
        {
            throw std::runtime_error("Registry::GetComponent failed: entity is not alive.");
        }

        const ComponentStorage<TComponent>& storage = GetComponentStorage<TComponent>();
        return storage.Get(entity);
    }
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H