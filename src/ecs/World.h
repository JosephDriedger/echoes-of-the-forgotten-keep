//
// Created by adamd on 2026-01-21.
//

#ifndef ASSIGNMENT1_WORLD_H
#define ASSIGNMENT1_WORLD_H
#include <memory>
#include <vector>
#include <manager/SceneType.h>

#include "AnimationSystem.h"
#include "Animation3DSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "CombatSystem.h"
#include "DamageSystem.h"
#include "DestructionSystem.h"
#include "EnemyAISystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "event/EventManager.h"
#include "KeyboardInputSystem.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MouseInputSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "SpawnTimerSystem.h"
#include "UIRenderSystem.h"


class World {
private:
    Map map;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    KeyboardInputSystem keyboardInputSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    EventManager eventManager;
    SpawnTimerSystem spawnTimerSystem;
    DestructionSystem destructionSystem;
    Animation3DSystem animation3DSystem;
    EventResponseSystem eventResponseSystem{*this};
    MainMenuSystem mainMenuSystem;
    EnemyAISystem enemyAISystem;
    DamageSystem damageSystem;
    CombatSystem combatSystem {*this};
    UIRenderSystem uiRenderSystem;
    MouseInputSystem mouseInputSystem;

public:
    World();
    void update(const float dt, const SDL_Event& event, const SceneType sceneType) {
        keyboardInputSystem.update(entities, event);
        movementSystem.update(entities, dt);
        collisionSystem.update(*this);
        animationSystem.update(entities, dt);
        cameraSystem.update(entities);
        spawnTimerSystem.update(entities, dt);
        animation3DSystem.update(entities, dt);
        enemyAISystem.update(entities, dt);
        damageSystem.update(entities);
        combatSystem.update(entities);
        destructionSystem.update(entities);
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

// static void onCollision(const CollisionEvent& collision);
#endif //ASSIGNMENT1_WORLD_H