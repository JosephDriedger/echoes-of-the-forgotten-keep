//
// Created by scept on 2026-03-09.
//

#include "DamageSystem.h"

#include "Entity.h"
#include "Component.h"
#include <iostream>

#include "Game.h"

void DamageSystem::update(std::vector<std::unique_ptr<Entity>>& entities)
{
    for (auto& e : entities)
    {
        if (!e->hasComponent<DamageEvent>()) {continue;}

        auto& dmg = e->getComponent<DamageEvent>();

        if (dmg.target && dmg.target->hasComponent<Health>())
        {
            auto& health = dmg.target->getComponent<Health>();

            health.currentHealth -= dmg.amount;
            if (dmg.target->hasComponent<PlayerTag>()) {
                Game::gameState.playerHealth = health.currentHealth;
            }

            std::cout << "Damage applied: " << dmg.amount
                      << " remaining HP: " << health.currentHealth
                      << std::endl;

            if (health.currentHealth <= 0)
            {
                if (dmg.target->hasComponent<Animator>()) {
                    auto& anim = dmg.target->getComponent<Animator>();
                    anim.isDead = true;
                }
                if (dmg.target->hasComponent<PlayerTag>()) {
                    Game::onSceneChangeRequest("gameover");
                }
            }
        }

        // remove the transient damage entity
        e->destory();
    }
}
