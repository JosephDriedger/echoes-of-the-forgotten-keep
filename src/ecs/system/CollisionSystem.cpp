//
// Created by adamd on 2026-02-04.
//

# include "CollisionSystem.h"

#include <iostream>

#include "Collision.h"
#include "World.h"

// has two important functions, but they are related
// 1. first is positions the collider with the transform
// 2. checking for collisions

void CollisionSystem::update(World &world) {

    // get a list of entities that have colliders and transforms
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());

    // update all collider positions first
    for (auto& entity : collidables) {
        auto& t = entity->getComponent<Transform>();
        auto& c = entity->getComponent<Collider>();
        c.rect.x = t.position.x;
        c.rect.y = t.position.y;
    }

    // outer loop
    for (size_t i = 0; i < collidables.size(); i++) {
        //update the collider position
        auto& entityA = collidables[i];
        auto& transformA = entityA->getComponent<Transform>();
        auto& colliderA = entityA->getComponent<Collider>();

        colliderA.rect.x = transformA.position.x;
        colliderA.rect.y = transformA.position.y;

        // check for the collider collision
        // inner loop
        for (size_t j = i + 1; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto& colliderB = entityB->getComponent<Collider>();

            if (Collision::AABB(colliderA, colliderB)) {
                //std::cout << colliderA.tag << " Hit: " << colliderB.tag << std::endl;
                world.getEventManager().emit(CollisionEvent{entityA,entityB});
            }
        }
    }
}

std::vector<Entity*> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities) {
    std::vector<Entity*> collidables;
    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            collidables.push_back(e.get());
        }
    }

    return collidables;
}
