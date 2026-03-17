//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H

#include <array>
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "engine/ecs/Component.h"
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

        void ClearComponentRegistrations();
        [[nodiscard]] std::size_t GetRegisteredComponentCount() const;

    private:
        [[nodiscard]] bool IsInRange(EntityId entityId) const;
        void RebuildActiveEntityList();

        std::queue<EntityId> m_AvailableEntityIds;
        std::array<bool, MAX_ENTITIES + 1> m_Alive;
        std::array<Signature, MAX_ENTITIES + 1> m_Signatures;
        std::vector<Entity> m_ActiveEntities;
        std::size_t m_AliveEntityCount;

        std::unordered_map<std::type_index, ComponentType> m_ComponentTypes;
        ComponentType m_NextComponentType;
    };

    template <typename TComponent>
    void Registry::RegisterComponent()
    {
        const std::type_index typeIndex(typeid(TComponent));

        if (m_ComponentTypes.find(typeIndex) != m_ComponentTypes.end())
        {
            return;
        }

        if (m_NextComponentType >= MAX_COMPONENTS)
        {
            throw std::runtime_error("Registry::RegisterComponent failed: maximum component count reached.");
        }

        m_ComponentTypes.emplace(typeIndex, m_NextComponentType);
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
        return m_ComponentTypes.find(typeIndex) != m_ComponentTypes.end();
    }
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_REGISTRY_H