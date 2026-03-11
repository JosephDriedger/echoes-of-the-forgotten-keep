//
// Created by adamd on 2026-02-18.
//

#ifndef ASSIGNMENT1_CAMERASYSTEM_H
#define ASSIGNMENT1_CAMERASYSTEM_H
#include <memory>

#include "Component.h"
#include "Entity.h"

class CameraSystem {
    public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* playerEntity = nullptr;

        // find the player
        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                playerEntity = e.get();
                break;
            }
        }

        if (!playerEntity) return;

        auto& playerTransform = playerEntity->getComponent<Transform3D>();

        for (auto& e : entities) {
            if (e->hasComponent<Camera3D>()) {

                auto& cam = e->getComponent<Camera3D>();
                auto& transform = e->getComponent<Transform3D>();

                // this position the camera so the player is at the center of its view
                // follow player
                transform.position.x = playerTransform.position.x;
                transform.position.y = playerTransform.position.y + 12.0f;
                transform.position.z = playerTransform.position.z + 12.0f;

                // look at player
                glm::vec3 direction = playerTransform.position - transform.position;
                cam.forward = glm::normalize(direction);

                // the camera is position itself so player is centered, but the player could walk off the screen
                // so clamp the camera so it stays within the window



            }
        }
    }
};

#endif //ASSIGNMENT1_CAMERASYSTEM_H