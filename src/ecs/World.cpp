//
// Created by adamd on 2026-02-04.
//

#include "World.h"

#include <iostream>

#include "Game.h"

World::World() {

    eventManager.subscribe(onCollision);

    // subscribe to the collision events
    eventManager.subscribe([this](const CollisionEvent& collision) {

        Entity* sceneStateEntity = nullptr;

        // find scene state
        for (auto& e : entities) {
            if (e->hasComponent<SceneState>()) {
                sceneStateEntity = e.get();
                break;
            }
        }

        if (!sceneStateEntity) return;

        if (collision.entityA == nullptr || collision.entityB == nullptr) return;

        if (!(collision.entityA->hasComponent<Collider>() && collision.entityB->hasComponent<Collider>())) return;

        auto& colliderA = collision.entityA->getComponent<Collider>();
        auto& colliderB = collision.entityB->getComponent<Collider>();

        Entity* player = nullptr;
        Entity* item = nullptr;
        Entity* wall = nullptr;
        Entity* projectile = nullptr;


        if (colliderA.tag == "player" && colliderB.tag == "item") {
            player = collision.entityA;
            item = collision.entityB;
        } else if (colliderA.tag == "item" && colliderB.tag == "player") {
            player = collision.entityB;
            item = collision.entityA;
        }

        if (player && item) {
            item->destory();
            //scene state
            auto& sceneState = sceneStateEntity->getComponent<SceneState>();
            sceneState.coinsCollected++;
            if (sceneState.coinsCollected > 1) {
                Game::onSceneChangeRequest("level2");
            }
        }

        // Player vs wall
        if (colliderA.tag == "player" && colliderB.tag == "wall") {
            player = collision.entityA;
            wall = collision.entityB;
        } else if (colliderA.tag == "wall" && colliderB.tag == "player") {
            player = collision.entityB;
            wall = collision.entityA;
        }

        if (player && wall) {
            //stop the player
            auto& t = player->getComponent<Transform>();
            t.position = t.oldPosition;
        }

        // Player vs projectile
        if (colliderA.tag == "player" && colliderB.tag == "projectile") {
            player = collision.entityA;
            projectile = collision.entityB;
        } else if (colliderA.tag == "projectile" && colliderB.tag == "player") {
            player = collision.entityB;
            projectile = collision.entityA;
        }

        if (player && projectile) {
            player->destory();
            // change scene deferred
            Game::onSceneChangeRequest("gameover");
        }
    });
}

static void onCollision(const CollisionEvent& collision) {

    if (collision.entityA == nullptr || collision.entityB == nullptr) return;

    if (!(collision.entityA->hasComponent<Collider>() && collision.entityB->hasComponent<Collider>())) return;

    auto& colliderA = collision.entityA->getComponent<Collider>();
    auto& colliderB = collision.entityB->getComponent<Collider>();

    Entity* entityA = collision.entityA;
    Entity* entityB = collision.entityB;

    //std::cout << "A collision occurred between " << colliderA.tag << " and " << colliderB.tag << std::endl;
}
