//
// Created by scept on 2026-03-04.
//

#include "EventResponseSystem.h"

#include "Game.h"
#include "World.h"

EventResponseSystem::EventResponseSystem(World &world) {
    // subscriptions
    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {

            if (e.type != EventType::Collision) return;
            const auto& collision = static_cast<const CollisionEvent&>(e); //cast base type to collision type

            onCollision(collision, world);

        }
    );

    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
            if (e.type != EventType::PlayerAction) return;
            const auto& playerAction = static_cast<const PlayerActionEvent&>(e);
                //TODO onPlayerAction
        }
    );

    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
            if (e.type != EventType::MouseInteraction) return;
            const auto& mouseInteractionEvent = static_cast<const MouseInteractionEvent&>(e);
            onMouseInteraction(mouseInteractionEvent);
        }
    );
}

void EventResponseSystem::onMouseInteraction(const MouseInteractionEvent& e) {
    if (!e.entity->hasComponent<Clickable>()) return;

    auto& clickable = e.entity->getComponent<Clickable>();

    switch (e.state) {
        case MouseInteractionState::Pressed:
            clickable.onPressed();
            break;
        case MouseInteractionState::Released:
            clickable.onReleased();
            break;
        case MouseInteractionState::Cancel:
            clickable.onCancel();
            break;
        default:
            break;
    }
}

void EventResponseSystem::onCollision(const CollisionEvent& e, World& world) {

    Entity* a = e.entityA;
    Entity* b = e.entityB;

    // if (!getCollisionEntities(e, otherTag, a, b)) return;

    if (!a || !b) return;

    // --------------------------------
    // 1. Generic damage collisions
    // --------------------------------
    applyDamageIfNeeded(e, a, b, world);
    applyDamageIfNeeded(e, b, a, world);

    // --------------------------------
    // 2. Special gameplay collisions
    // --------------------------------
    handleItemPickup(e, world);
    handleWallCollision(e);
    handleButtonPress(e);

    // if (std::string(otherTag) == "item") {
    //
    //     if (e.state != CollisionState::Enter) return;
    //
    //     other->destroy();
    //     // scene state
    //
    //     for (auto& entity : world.getEntities()) {
    //         if (!entity->hasComponent<SceneState>()) continue;
    //         auto& sceneState = entity->getComponent<SceneState>();
    //         sceneState.coinsCollected++;
    //         if (sceneState.coinsCollected > 1) {
    //             Game::onSceneChangeRequest("level2");
    //         }
    //     }
    // } else if (std::string(otherTag) == "wall") {
    //
    //     if (e.state != CollisionState::Stay) return;
    //
    //     // stop the player
    //     auto& t = player->getComponent<Transform>();
    //     t.position = t.oldPosition;
    // } else if (std::string(otherTag) == "projectile") {
    //
    //     if (e.state != CollisionState::Enter) return;
    //
    //     // this logic is simple and direct
    //     // ideally we would only operate data in an update function (hinting at transient entities)
    //     // create damage entity and look for damage entity in update
    //     auto& health = player->getComponent<Health>();
    //     // health.currentHealth--;
    //     auto& dmg = world.createDeferredEntity();
    //     dmg.addComponent<DamageEvent>(player, 1);
    //
    //     // Game::gameState.playerHealth = health.currentHealth;
    //
    //     // std::cout << "health currentHealth: " << health.currentHealth << std::endl;
    //     // if (health.currentHealth <= 0) {
    //     //     player->destroy();
    //     //     Game::onSceneChangeRequest("gameover");
    //     // }
    //     // player->destroy();
    //     // change scenes defer
    //     // Game::onSceneChangeRequest("gameover");
    // }
}

bool EventResponseSystem::getCollisionEntities(
    const CollisionEvent &e,
    const char *otherTag,
    Entity *&player,
    Entity *&other) {

    if (e.entityA == nullptr || e.entityB == nullptr) return false;

    if (!e.entityA->hasComponent<Collider>() && e.entityB->hasComponent<Collider>()) return false;

    auto& colliderA = e.entityA->getComponent<Collider>();
    auto& colliderB = e.entityB->getComponent<Collider>();

    if (colliderA.tag == "player" && colliderB.tag == otherTag) {
        player = e.entityA;
        other = e.entityB;
    } else if (colliderA.tag == otherTag && colliderB.tag == "player") {
        player = e.entityB;
        other = e.entityA;
    }

    return player && other;
}

void EventResponseSystem::applyDamageIfNeeded(const CollisionEvent& e, Entity* attacker, Entity* target, World& world)
{
    if (e.state != CollisionState::Enter) return;
    if (!attacker->hasComponent<Damage>() || attacker->getComponent<Parent>().entity == target) {
        return;
    }

    if (!target->hasComponent<Health>()) {
        return;
    }

    auto& dmg = attacker->getComponent<Damage>();

    if (attacker->hasComponent<ProjectileTag>()) {
        // auto& projectile = attacker->getComponent<ProjectileTag>();
        // if (projectile.melee) {
        //     if (projectile.active) {
        //         projectile.active = false;
        //         std::cout << "Hitbox Inactive" << std::endl;
        //     }
        //     return;
        // }
        std::cout << "Applying Damage" << std::endl;
        auto& event = world.createDeferredEntity();
        event.addComponent<DamageEvent>(target, dmg.amount);
        attacker->destory();
    }
}

void EventResponseSystem::handleItemPickup(const CollisionEvent& e, World& world)
{
    Entity* player = nullptr;
    Entity* item = nullptr;

    if (!getTaggedCollisionEntities(e, "player", "item", player, item))
        return;

    if (e.state != CollisionState::Enter)
        return;

    item->destory();

    for (auto& entity : world.getEntities())
    {
        if (!entity->hasComponent<SceneState>()) continue;

        auto& sceneState = entity->getComponent<SceneState>();
        sceneState.coinsCollected++;

        if (sceneState.coinsCollected > 1)
            Game::onSceneChangeRequest("level2");
    }
}

void EventResponseSystem::handleWallCollision(const CollisionEvent& e)
{
    Entity* player = nullptr;
    Entity* wall = nullptr;

    if (!getTaggedCollisionEntities(e, "player", "wall", player, wall))
        return;

    if (e.state != CollisionState::Stay)
        return;

    // stop the player
    auto& t = player->getComponent<Transform>();
    t.position = t.oldPosition;
}

void EventResponseSystem::handleButtonPress(const CollisionEvent &e) {
    // if (e.state != CollisionState::Enter) return;

    if (!e.entityA->hasComponent<PlayerTag>() || !e.entityB->hasComponent<PlayerTag>()) return;
    if (!e.entityA->hasComponent<Switch>() || !e.entityB->hasComponent<Switch>()) return;

    Entity* player = nullptr;
    Entity* button = nullptr;

    if (e.entityA->hasComponent<PlayerTag>()) {
        player = e.entityA;
    } else if (e.entityB->hasComponent<PlayerTag>()) {
        player = e.entityB;
    }

    if (e.entityA->hasComponent<Switch>()) {
        button = e.entityA;
    } else if (e.entityB->hasComponent<Switch>()) {
        button = e.entityB;
    }

    auto& trigger = button->getComponent<Switch>();

    if (!trigger.pressed && e.state == CollisionState::Enter) {
        trigger.pressed = true;
        std::cout << "Button pressed!" << std::endl;
    }
}

bool EventResponseSystem::getTaggedCollisionEntities(
    const CollisionEvent& e,
    const char* tagA,
    const char* tagB,
    Entity*& a,
    Entity*& b)
{
    if (!e.entityA || !e.entityB) return false;

    if (!e.entityA->hasComponent<Collider>() ||
        !e.entityB->hasComponent<Collider>())
        return false;

    auto& colA = e.entityA->getComponent<Collider>();
    auto& colB = e.entityB->getComponent<Collider>();

    if (colA.tag == tagA && colB.tag == tagB)
    {
        a = e.entityA;
        b = e.entityB;
    }
    else if (colA.tag == tagB && colB.tag == tagA)
    {
        a = e.entityB;
        b = e.entityA;
    }

    return a && b;
}