//
// Created by scept on 2026-03-04.
//

#ifndef INC_8051TUTORIAL_EVENTRESPONSESYSTEM_H
#define INC_8051TUTORIAL_EVENTRESPONSESYSTEM_H
#include <functional>

#include "event/BaseEvent.h"

class Entity;
class World;

class EventResponseSystem {
public:
    EventResponseSystem(World &world);
private:
    // collisions
    void onCollision(const CollisionEvent& e, World& world);
    bool getCollisionEntities(const CollisionEvent& e, const char* otherTag, Entity*& player, Entity*& other);
    bool getTaggedCollisionEntities(const CollisionEvent& e, const char* tagA, const char* tagB, Entity*& a, Entity*& b);

    // player action
    void onPlayerAction(const PlayerActionEvent& e, const std::function<void(Entity* player, PlayerAction action)>& callback);

    void onMouseInteraction(const MouseInteractionEvent& e);

    void applyDamageIfNeeded(const CollisionEvent& e, Entity* attacker, Entity* target, World& world);
    void handleItemPickup(const CollisionEvent& e, World& world);
    void handleWallCollision(const CollisionEvent& e);
    void handleButtonPress(const CollisionEvent& e);
};

#endif //INC_8051TUTORIAL_EVENTRESPONSESYSTEM_H