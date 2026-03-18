//
// Created by adamd on 2026-02-04.
//

#ifndef ASSIGNMENT1_COLLISIONSYSTEM_H
#define ASSIGNMENT1_COLLISIONSYSTEM_H
#include <algorithm>
#include <vector>
#include <memory>
#include <set>

#include "Entity.h"

using CollisionKey = std::pair<Entity*, Entity*>;

// forward declaring
class World;

class CollisionSystem {
public:
    void update(World& world);
    std::set<CollisionKey> activeCollisions;
private:
    std::vector<Entity*> queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities);
    std::vector<Entity*> queryCollidables3D(const std::vector<std::unique_ptr<Entity>>& entities);
    CollisionKey makeKey(Entity* entityA, Entity* entityB) {
        return std::minmax(entityA, entityB); // automatically orders our pair so smaller number (memory address) is ordered first
    }
};

#endif //ASSIGNMENT1_COLLISIONSYSTEM_H