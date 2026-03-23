//
// Created by adamd on 2026-01-21.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MOVEMENTSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MOVEMENTSYSTEM_H
#include <memory>
#include <vector>

#include "../Component.h"
#include "../Entity.h"

class MovementSystem {
    public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const float dt) {
        for (auto& entity : entities) {
            if (entity->hasComponent<Transform3D>() && entity->hasComponent<Velocity3D>()) {
                auto& t = entity->getComponent<Transform3D>();
                auto& v = entity->getComponent<Velocity3D>();

                // track previous frames position
                t.oldPosition = t.position;

                glm::vec3 directionVec = v.direction;

                // normalizing
                if (glm::length(directionVec) > 0.0001f) {
                    directionVec = glm::normalize(directionVec);
                }

                // Vector2D needs an operator function to multiply a float to itself
                glm::vec3 velocityVector = directionVec * v.speed;

                // t.position = t.position + velocityVector * dt
                t.position += velocityVector * dt;
            }
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_MOVEMENTSYSTEM_H