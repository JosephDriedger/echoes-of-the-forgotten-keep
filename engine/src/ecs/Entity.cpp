// Created by Joey Driedger

#include "engine/ecs/Entity.h"

namespace engine
{
    Entity::Entity()
        : m_Id(INVALID_ENTITY_ID)
    {
    }

    Entity::Entity(const EntityId id)
        : m_Id(id)
    {
    }

    EntityId Entity::GetId() const
    {
        return m_Id;
    }

    bool Entity::IsValid() const
    {
        return m_Id != INVALID_ENTITY_ID;
    }

    Entity::operator bool() const
    {
        return IsValid();
    }

    bool Entity::operator==(const Entity& other) const
    {
        return m_Id == other.m_Id;
    }

    bool Entity::operator!=(const Entity& other) const
    {
        return m_Id != other.m_Id;
    }

    bool Entity::operator<(const Entity& other) const
    {
        return m_Id < other.m_Id;
    }
}