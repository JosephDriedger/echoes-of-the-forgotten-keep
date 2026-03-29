//
// Created by scept on 2026-03-25.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_LIFETIMESYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_LIFETIMESYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class LifetimeSystem {
    public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt)
    {
        for (auto& e : entities)
        {
            if (!e->hasComponent<ProjectileTag>()) continue;

            auto& life = e->getComponent<ProjectileTag>();
            life.duration -= dt;

            if (life.duration <= 0)
            {
                e->destory();
            }
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_LIFETIMESYSTEM_H