//
// Created by adamd on 2026-01-21.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_WORLD_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_WORLD_H
#include <memory>
#include <vector>

#include "Animation3DSystem.h"
#include "AttachmentSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "DestructionSystem.h"
#include "Entity.h"
#include "EventManager.h"
#include "KeyboardInputSystem.h"
#include "../Map.h"
#include "MovementSystem.h"
#include "RenderSystem.h"

class World {
private:
    Map map;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    KeyboardInputSystem keyboardInputSystem;
    CollisionSystem collisionSystem;
    CameraSystem cameraSystem;
    EventManager eventManager;
    DestructionSystem destructionSystem;
    Animation3DSystem animation3DSystem;
    AttachmentSystem attachmentSystem;
public:
    World();
    void update(const float dt, const SDL_Event& event) {
        keyboardInputSystem.update(entities, event);
        movementSystem.update(entities, dt);
        collisionSystem.update(*this);
        cameraSystem.update(entities);
        destructionSystem.update(entities);
        animation3DSystem.update(entities, dt);
        attachmentSystem.update(entities);
        synchronizeEntities();
        cleanup();
    }

    void render() {

        for (auto& entity : entities) {
            if (entity->hasComponent<Camera>()) {
                map.draw(entity->getComponent<Camera>());
                break;
            }
        }

        renderSystem.render(entities);
    }

    Entity& createEntity() {
        // we use emplace instead of push so don't create copy
        entities.emplace_back(std::make_unique<Entity>());
        return *entities.back();
    }

    Entity& createDeferredEntity() {
        deferredEntities.emplace_back(std::make_unique<Entity>());
        return *deferredEntities.back();
    }

    std::vector<std::unique_ptr<Entity>>& getEntities() {
        return entities;
    }

    void cleanup() {
        //use a lambda predicate to remove all inactive entities
        std::erase_if(
            entities,
            [](const std::unique_ptr<Entity>& e) {
                return !e->isActive();
            }
        );
    }

    void synchronizeEntities() {
        if (!deferredEntities.empty()) {
            // push back all deferred entities to the entities vector
            // using move so we don't create a copy
            std::move(
                deferredEntities.begin(),
                deferredEntities.end(),
                std::back_inserter(entities)
                );
            // clearing the creation buffer
            deferredEntities.clear();
        }
    }

    EventManager& getEventManager() {
        return eventManager;
    }
    Map& getMap() {return map;}
};

static void onCollision(const CollisionEvent& collision);
#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_WORLD_H