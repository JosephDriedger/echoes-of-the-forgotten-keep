//
// Created by adamd on 2026-02-04.
//

# include "CollisionSystem.h"

#include <iostream>

#include "Collision.h"
#include "../World.h"

// has two important functions, but they are related
// 1. first is positions the collider with the transform
// 2. checking for collisions

void CollisionSystem::update(World &world) {

    // get a list of entities that have colliders and transforms
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());
    const std::vector<Entity*> collidables3D = queryCollidables3D(world.getEntities());

    // update all collider positions first
    for (auto& entity : collidables) {
        auto& t = entity->getComponent<Transform>();
        auto& c = entity->getComponent<Collider>();
        c.rect.x = t.position.x;
        c.rect.y = t.position.y;
    }

    std::set<CollisionKey> currentCollisions;

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
                //std::cout << colliderA.tag << " hit: " << colliderB.tag << std::endl;
                CollisionKey key = makeKey(entityA, entityB);
                currentCollisions.insert(key);
                if (!activeCollisions.contains(key)) {
                    world.getEventManager().emit(CollisionEvent{entityA, entityB, CollisionState::Enter});
                }
                world.getEventManager().emit(CollisionEvent{entityA, entityB, CollisionState::Stay});
            }
        }
    }

    for (size_t i = 0; i < collidables3D.size(); i++) {
        auto entityA3D = collidables3D[i];
        auto& transformA3D = entityA3D->getComponent<Transform3D>();
        auto& colliderA3D = entityA3D->getComponent<Collider3D>();

        for (size_t j = i + 1; j < collidables3D.size(); j++) {
            auto entityB3D = collidables3D[j];
            auto& transformB3D = entityB3D->getComponent<Transform3D>();
            auto& colliderB3D = entityB3D->getComponent<Collider3D>();

            if (Collision::AABB3D(transformA3D, colliderA3D, transformB3D, colliderB3D)) {
                CollisionKey key = makeKey(entityA3D, entityB3D);
                currentCollisions.insert(key);

                if (!activeCollisions.contains(key)) {
                    world.getEventManager().emit(
                        CollisionEvent{entityA3D, entityB3D, CollisionState::Enter});
                    std::cout << "Collision Enter" << std::endl;
                }

                world.getEventManager().emit(
                    CollisionEvent{entityA3D, entityB3D, CollisionState::Stay});
            }
        }
    }

    for (auto& key : activeCollisions) {
        if (!currentCollisions.contains(key)) {
            world.getEventManager().emit(CollisionEvent{key.first, key.second, CollisionState::Exit});
        }
    }

    activeCollisions = std::move(currentCollisions); // update with current collisions

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

std::vector<Entity*> CollisionSystem::queryCollidables3D(const std::vector<std::unique_ptr<Entity>>& entities) {
    std::vector<Entity*> collidables;
    for (auto& e : entities) {
        if (e->hasComponent<Transform3D>() && e->hasComponent<Collider3D>()) {
            collidables.push_back(e.get());
        }
    }

    return collidables;
}
