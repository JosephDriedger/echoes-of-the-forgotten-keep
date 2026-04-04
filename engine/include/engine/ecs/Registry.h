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
    class Registry
    {
    public:
        Registry();

        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        void DestroyAllEntities();

        [[nodiscard]] bool IsAlive(Entity entity) const;

        void Reset();

        [[nodiscard]] std::size_t GetAliveCount() const;
        [[nodiscard]] std::size_t GetAvailableCount() const;

        void SetSignature(Entity entity, const Signature& signature);
        [[nodiscard]] const Signature& GetSignature(Entity entity) const;
        void ClearSignature(Entity entity);

        [[nodiscard]] const std::vector<Entity>& GetActiveEntities() const;

        template <typename TComponent>
        void RegisterComponent();

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

        template <typename TComponent>
        ComponentStorage<TComponent>& GetComponentStorage();

        template <typename TComponent>
        const ComponentStorage<TComponent>& GetComponentStorage() const;

        std::stack<EntityId> m_AvailableEntityIds;
        std::array<bool, MAX_ENTITIES + 1> m_Alive;
        std::array<Signature, MAX_ENTITIES + 1> m_Signatures;
        std::array<std::size_t, MAX_ENTITIES + 1> m_ActiveEntityIndices;
        std::vector<Entity> m_ActiveEntities;
        std::size_t m_AliveEntityCount;

        std::unordered_map<std::type_index, ComponentType> m_ComponentTypes;
        std::array<std::shared_ptr<IComponentStorage>, MAX_COMPONENTS> m_ComponentStorages;
        ComponentType m_NextComponentType;
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