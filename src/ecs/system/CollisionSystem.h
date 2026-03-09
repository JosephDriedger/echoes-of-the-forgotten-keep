//
// Created by adamd on 2026-02-04.
//

#ifndef ASSIGNMENT1_COLLISIONSYSTEM_H
#define ASSIGNMENT1_COLLISIONSYSTEM_H
#include <memory>
#include <vector>

#include "Entity.h"

// forward declaring
class World;

class CollisionSystem {
public:
    void update(World& world);
private:
    std::vector<Entity*> queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities);
};

#endif //ASSIGNMENT1_COLLISIONSYSTEM_H