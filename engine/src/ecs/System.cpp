// Created by Joey Driedger

#include "engine/ecs/System.h"

namespace engine
{
    const std::set<Entity>& System::GetEntities() const
    {
        return m_Entities;
    }
}