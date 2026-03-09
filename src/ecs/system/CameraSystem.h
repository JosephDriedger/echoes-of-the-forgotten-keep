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

        auto& playerTransform = playerEntity->getComponent<Transform>();

        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {

                auto& cam = e->getComponent<Camera>();

                // this position the camera so the player is at the center of its view
                cam.view.x = playerTransform.position.x - cam.view.w / 2;
                cam.view.y = playerTransform.position.y - cam.view.h / 2;

                // the camera is position itself so player is centered, but the player could walk off the screen
                // so clamp the camera so it stays within the window
                if (cam.view.x < 0) {
                    cam.view.x = 0;
                }
                if (cam.view.y < 0) {
                    cam.view.y = 0;
                }
                if (cam.view.x > cam.worldWidth - cam.view.w) {
                    cam.view.x = cam.worldWidth - cam.view.w;
                }
                if (cam.view.y > cam.worldHeight - cam.view.h) {
                    cam.view.y = cam.worldHeight - cam.view.h;
                }


            }
        }
    }
};

#endif //ASSIGNMENT1_CAMERASYSTEM_H