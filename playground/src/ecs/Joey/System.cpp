//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/ecs/System.h"

namespace engine
{
    const std::set<Entity>& System::GetEntities() const
    {
        return m_Entities;
    }
}