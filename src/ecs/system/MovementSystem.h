//
// Created by adamd on 2026-01-21.
//

#ifndef ASSIGNMENT1_MOVEMENTSYSTEM_H
#define ASSIGNMENT1_MOVEMENTSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class MovementSystem {
    public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
                auto& t = entity->getComponent<Transform>();
                auto& v = entity->getComponent<Velocity>();

                // track previous frames position
                t.oldPosition = t.position;

                Vector2D directionVec = v.direction;

                // normalizing
                directionVec.normalize();

                // Vector2D needs an operator function to multiply a float to itself
                Vector2D velocityVector = directionVec * v.speed;

                // t.position = t.position + velocityVector * dt
                t.position += velocityVector * dt;


            }
        }
    }
};

#endif //ASSIGNMENT1_MOVEMENTSYSTEM_H