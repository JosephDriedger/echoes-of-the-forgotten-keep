//
// Created by adamd on 2026-02-25.
//

#ifndef ASSIGNMENT1_SPAWNTIMERSYSTEM_H
#define ASSIGNMENT1_SPAWNTIMERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class SpawnTimerSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<TimedSpawner>()) {
                auto& spawner = entity->getComponent<TimedSpawner>();

                spawner.timer -= dt;

                if (spawner.timer <= 0) {
                    spawner.timer = spawner.spawnInterval; // (eg 2 seconds)
                    spawner.spawnCallback(); // crearte entity inside here
                }
            }
        }
    }
};

#endif //ASSIGNMENT1_SPAWNTIMERSYSTEM_H