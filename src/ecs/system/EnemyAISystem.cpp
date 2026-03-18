//
// Created by scept on 2026-03-09.
//

#include "EnemyAISystem.h"

#include <cmath>
#include <iostream>

std::vector<Entity*> EnemyAISystem::queryEnemies(const std::vector<std::unique_ptr<Entity>>& entities) {

    std::vector<Entity*> enemies;

    for (auto& e : entities) {

        if (e->hasComponent<EnemyTag>() &&
            e->hasComponent<Transform>() &&
            e->hasComponent<Velocity>() &&
            e->hasComponent<AI>())
        {
            enemies.push_back(e.get());
        }
    }

    return enemies;
}

void EnemyAISystem::update(std::vector<std::unique_ptr<Entity> > &entities, float dt) {
    Entity* player = findPlayer(entities);

    auto enemies = queryEnemies(entities);

    for (auto enemy : enemies)
    {
        auto& ai = enemy->getComponent<AI>();

        ai.stateTimer += dt;

        switch (ai.state)
        {
            case AIState::Idle:
                UpdateIdle(enemy, ai, dt);
                break;

            case AIState::Patrol:
                UpdatePatrol(enemy, ai, dt);
                break;

            case AIState::Chase:
                UpdateChase(enemy, ai, dt);
                break;

            case AIState::Attack:
                UpdateAttack(enemy, ai, dt);
                break;
        }

        // global detection check (can interrupt idle/patrol)
        if (player)
        {
            auto& enemyTransform = enemy->getComponent<Transform>();
            auto& playerTransform = player->getComponent<Transform>();

            float dist = distance(enemyTransform, playerTransform);

            if (ai.loseTargetTimer > 0)
            {
                ai.loseTargetTimer -= dt;
                ai.target = nullptr;
            } else if (dist < ai.visionRange &&
                ai.loseTargetTimer <= 0 &&
                (ai.state == AIState::Idle || ai.state == AIState::Patrol))
            {
                ai.state = AIState::Chase;
                ai.target = player;
            }
        }
    }
}

void EnemyAISystem::UpdateAttack(Entity* enemy, AI &ai, float dt) {
    auto& velocity = enemy->getComponent<Velocity>();
    velocity.direction = {0, 0};
    velocity.speed = 0;

    if (!enemy->hasComponent<Combat>()) {return;}

    auto& combat = enemy->getComponent<Combat>();

    // cooldown
    combat.attack.timer += dt;
    if (combat.attack.timer >= combat.attack.cooldown) {
        combat.attack.timer = 0;

        // --- perform attack here ---
        // e.g., spawn projectile, deal damage to player, play animation
        // Example:
        // spawnProjectile(enemy->getComponent<Transform>().position, ai.target);

        // For now we just print
        std::cout << "Enemy attacks player!" << std::endl;
        // spawnProjectile(enemy, ai.target);
        auto& targetTransform = ai.target->getComponent<Transform>();
        Vector2D dir = (targetTransform.position - enemy->getComponent<Transform>().position).normalized();
        auto& req = enemy->addComponent<AttackRequest>();
        req.direction = dir;
        req.tag = "projectile";
    }

    // check distance to player
    if (!ai.target || !ai.target->hasComponent<Transform>()) {
        ai.state = AIState::Idle;
        ai.stateTimer = 0;
        ai.target = nullptr;
        return;
    }

    auto& targetTransform = ai.target->getComponent<Transform>();
    auto& transform = enemy->getComponent<Transform>();
    float dist = (targetTransform.position - transform.position).length();

    if (dist > combat.attackRange) {
        // player moved away → chase again
        ai.state = AIState::Chase;
        ai.stateTimer = 0;
    }
}

void EnemyAISystem::UpdateChase(Entity* enemy, AI &ai, float dt) {
    if (!ai.target) {
        // lost target, go idle
        ai.state = AIState::Idle;
        ai.stateTimer = 0;
        return;
    }

    if (ai.stateTimer > ai.maxChaseTime)
    {
        ai.state = AIState::Idle;
        ai.stateTimer = 0;
        ai.target = nullptr;
        ai.loseTargetTimer = ai.loseTargetCooldown;
        return;
    }

    auto& transform = enemy->getComponent<Transform>();
    auto& velocity = enemy->getComponent<Velocity>();
    auto& combat = enemy->getComponent<Combat>();

    // make sure the target has a Transform
    if (!ai.target->hasComponent<Transform>()) return;
    auto& targetTransform = ai.target->getComponent<Transform>();

    Vector2D dir = targetTransform.position - transform.position;
    float dist = dir.length();

    if (dist <= combat.attackRange) {
        ai.state = AIState::Attack;
        ai.stateTimer = 0;
        return;
    }

    // optionally, lose target if player is far away
    if (dist > ai.visionRange * 1.5f) {
        ai.state = AIState::Idle;
        ai.stateTimer = 0;
        ai.target = nullptr;
        return;
    }

    // move toward player
    velocity.direction = dir.normalized();
    velocity.speed = 100; // chase speed
}

void EnemyAISystem::UpdateIdle(Entity* enemy, AI &ai, float dt) {
    auto& velocity = enemy->getComponent<Velocity>();

    velocity.direction = {0,0};
    velocity.speed = 0;

    // after a short time start patrol
    if (ai.stateTimer > 2.0f && enemy->hasComponent<Patrol>())
    {
        ai.state = AIState::Patrol;
        ai.stateTimer = 0;
    }
}

void EnemyAISystem::UpdatePatrol(Entity* enemy, AI &ai, float dt) {
    if (!enemy->hasComponent<Patrol>())
        return;

    auto& patrol = enemy->getComponent<Patrol>();
    auto& transform = enemy->getComponent<Transform>();
    auto& velocity = enemy->getComponent<Velocity>();

    Vector2D target = patrol.movingToB ? patrol.pointB : patrol.pointA;

    Vector2D dir = target - transform.position;

    float dist = dir.length();

    // reached patrol point
    if (dist < 5.0f)
    {
        patrol.movingToB = !patrol.movingToB;
        return;
    }

    velocity.direction = dir.normalized();
    velocity.speed = 60;
}

Entity* EnemyAISystem::findPlayer(const std::vector<std::unique_ptr<Entity>>& entities) {
    for (auto& e : entities) {
        if (e->hasComponent<PlayerTag>())
            return e.get();
    }
    return nullptr;
}

float EnemyAISystem::distance(const Transform& a, const Transform& b) {
    float dx = a.position.x - b.position.x;
    float dy = a.position.y - b.position.y;
    return std::sqrt(dx * dx + dy * dy);
}

