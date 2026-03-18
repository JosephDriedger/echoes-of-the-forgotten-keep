//
// Created by scept on 2026-03-11.
//

#ifndef INC_8051TUTORIAL_COMBATSYSTEM_H
#define INC_8051TUTORIAL_COMBATSYSTEM_H

#include <vector>
#include <memory>
#include "Entity.h"
#include "Vector2D.h"

class World;

class CombatSystem {
    public:
    CombatSystem(World& world) : world(world) {}

    void update(std::vector<std::unique_ptr<Entity>>& entities);

    private:
    World& world;

    void spawnProjectile(Entity* attacker, Vector2D direction, std::string tag);
};

#endif //INC_8051TUTORIAL_COMBATSYSTEM_H