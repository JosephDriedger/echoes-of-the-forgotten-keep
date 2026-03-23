//
// Created by adamd on 2026-02-25.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DESTRUCTIONSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DESTRUCTIONSYSTEM_H
#include <memory>
#include <vector>

#include "../Component.h"
#include "../Entity.h"

class DestructionSystem {
    public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {

        Entity* cameraEntity = nullptr;

        // find camera
        for (auto& entity : entities) {
            if (entity->hasComponent<Camera>()) {
                cameraEntity = entity.get();
                break;
            }
        }

        if (!cameraEntity) return;
        auto& cam = cameraEntity->getComponent<Camera>();

        for (auto& entity : entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<ProjectileTag>()) {
                auto& t = entity->getComponent<Transform>();
                // destroy entity if it goes out the cam view
                if (
                    t.position.x > cam.view.x + cam.view.w || // right edge of view
                    t.position.x < cam.view.x || // left edge
                    t.position.y > cam.view.y + cam.view.h || // bottom
                    t.position.y < cam.view.y) { // top

                    entity->destory();
                }
            }
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_DESTRUCTIONSYSTEM_H