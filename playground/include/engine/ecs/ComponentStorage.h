//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_STORAGE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_STORAGE_H

#include <cstddef>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine/ecs/Entity.h"

namespace engine
{
    class IComponentStorage
    {
    public:
        virtual ~IComponentStorage() = default;

        virtual void Remove(Entity entity) = 0;
        virtual void Clear() = 0;
    };

    template <typename TComponent>
    class ComponentStorage final : public IComponentStorage
    {
    public:
        void Add(Entity entity, const TComponent& component)
        {
            const EntityId entityId = entity.GetId();

            if (m_EntityToIndex.contains(entityId))
            {
                throw std::runtime_error("ComponentStorage::Add failed: component already exists for entity.");
            }

            const std::size_t newIndex = m_Components.size();

            m_Components.push_back(component);
            m_Entities.push_back(entity);
            m_EntityToIndex.emplace(entityId, newIndex);
        }

        template <typename... TArgs>
        TComponent& Emplace(Entity entity, TArgs&&... args)
        {
            const EntityId entityId = entity.GetId();

            if (m_EntityToIndex.contains(entityId))
            {
                throw std::runtime_error("ComponentStorage::Emplace failed: component already exists for entity.");
            }

            const std::size_t newIndex = m_Components.size();

            m_Components.emplace_back(std::forward<TArgs>(args)...);
            m_Entities.push_back(entity);
            m_EntityToIndex.emplace(entityId, newIndex);

            return m_Components.back();
        }

        void Remove(Entity entity) override
        {
            const EntityId entityId = entity.GetId();
            const auto iterator = m_EntityToIndex.find(entityId);

            if (iterator == m_EntityToIndex.end())
            {
                return;
            }

            const std::size_t removedIndex = iterator->second;
            const std::size_t lastIndex = m_Components.size() - 1;

            if (removedIndex != lastIndex)
            {
                m_Components[removedIndex] = std::move(m_Components[lastIndex]);
                m_Entities[removedIndex] = m_Entities[lastIndex];
                m_EntityToIndex[m_Entities[removedIndex].GetId()] = removedIndex;
            }

            m_Components.pop_back();
            m_Entities.pop_back();
            m_EntityToIndex.erase(iterator);
        }

        [[nodiscard]] bool Has(Entity entity) const
        {
            return m_EntityToIndex.contains(entity.GetId());
        }

        TComponent& Get(const Entity entity)
        {
            const auto iterator = m_EntityToIndex.find(entity.GetId());

            if (iterator == m_EntityToIndex.end())
            {
                throw std::runtime_error("ComponentStorage::Get failed: component does not exist for entity.");
            }

            return m_Components[iterator->second];
        }

        const TComponent& Get(Entity entity) const
        {
            const auto iterator = m_EntityToIndex.find(entity.GetId());

            if (iterator == m_EntityToIndex.end())
            {
                throw std::runtime_error("ComponentStorage::Get failed: component does not exist for entity.");
            }

            return m_Components[iterator->second];
        }

        void Clear() override
        {
            m_Components.clear();
            m_Entities.clear();
            m_EntityToIndex.clear();
        }

        [[nodiscard]] std::size_t GetCount() const
        {
            return m_Components.size();
        }

    private:
        std::vector<TComponent> m_Components;
        std::vector<Entity> m_Entities;
        std::unordered_map<EntityId, std::size_t> m_EntityToIndex;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_STORAGE_H