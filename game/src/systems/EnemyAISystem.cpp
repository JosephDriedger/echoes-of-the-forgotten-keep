//
// Created by scept on 2026-03-09.
//

#include "../../include/game/systems/EnemyAISystem.h"

#include <cmath>
#include <iostream>

std::vector<Entity*> EnemyAISystem::queryEnemies(const std::vector<std::unique_ptr<Entity>>& entities) {

    std::vector<Entity*> enemies;

    for (auto& e : entities) {

        if (e->hasComponent<EnemyTag>() &&
            e->hasComponent<Transform3D>() &&
            e->hasComponent<Velocity3D>() &&
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
        if (enemy->hasComponent<Health>()) {
            auto& health = enemy->getComponent<Health>();
            if (health.currentHealth <= 0) {
                return;
            }
        }
        auto& ai = enemy->getComponent<AI>();
        auto& animState = enemy->getComponent<Animator>();

        ai.stateTimer += dt;

        switch (ai.state)
        {
            case AIState::Idle:
                UpdateIdle(enemy, ai, animState, dt);
                break;

            case AIState::Patrol:
                UpdatePatrol(enemy, ai, animState, dt);
                break;

            case AIState::Chase:
                UpdateChase(enemy, ai, animState, dt);
                break;

            case AIState::Attack:
                UpdateAttack(enemy, ai, animState, dt);
                break;
        }

        // global detection check (can interrupt idle/patrol)
        if (player)
        {
            // auto& enemyTransform = enemy->getComponent<Transform>();
            // auto& playerTransform = player->getComponent<Transform>();
            auto& enemyTransform = enemy->getComponent<Transform3D>();
            auto& playerTransform = player->getComponent<Transform3D>();

            float dist = distance(enemyTransform, playerTransform);

            if (ai.loseTargetTimer > 0)
            {
                ai.loseTargetTimer -= dt;
                ai.target = nullptr;
            } else if (dist < ai.visionRange &&
                ai.loseTargetTimer <= 0 &&
                (ai.state == AIState::Idle || ai.state == AIState::Patrol) &&
                !animState.isAttacking)
            {
                ai.state = AIState::Chase;
                ai.target = player;
            }
        }
    }
}

void EnemyAISystem::UpdateAttack(Entity* enemy, AI &ai, Animator& animator, float dt) {
    auto& velocity = enemy->getComponent<Velocity3D>();
    velocity.direction = glm::vec3(0.0f);
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
        // animator.currentState = AnimState::Attack1;
        // std::cout << animator.isAttacking << std::endl;
        animator.isAttacking = true;
        // std::cout << animator.isAttacking << std::endl;
        // spawnProjectile(enemy, ai.target);
        if (!ai.target) {
            return;
        }
        auto& transform = enemy->getComponent<Transform3D>();
        glm::vec3 forward;
        forward.x = sin(glm::radians(transform.rotation.y));
        forward.z = cos(glm::radians(transform.rotation.y));
        forward.y = 0.0f;
        forward = glm::normalize(forward);
        glm::vec3 dir = forward;
        auto& req = enemy->addComponent<AttackRequest>();
        req.direction = dir;
        req.tag = "projectile";
    }

    // check distance to player
    if (!ai.target || !ai.target->hasComponent<Transform3D>() && !animator.isAttacking) {
        ai.state = AIState::Idle;
        ai.stateTimer = 0;
        ai.target = nullptr;
        return;
    }

    auto& targetTransform = ai.target->getComponent<Transform3D>();
    auto& transform = enemy->getComponent<Transform3D>();
    float dist = glm::distance(targetTransform.position, transform.position);

    float attackExitRange  = combat.attackRange + 1.0f;
    if (dist > attackExitRange && !animator.isAttacking) {
        // player moved away → chase again
        ai.state = AIState::Chase;
        ai.stateTimer = 0;
    }
}

void EnemyAISystem::UpdateChase(Entity* enemy, AI &ai, Animator& animator, float dt) {
    if (!ai.target && !animator.isAttacking) {
        // lost target, go idle
        ai.state = AIState::Idle;
        ai.stateTimer = 0;
        return;
    }

    if (ai.stateTimer > ai.maxChaseTime && !animator.isAttacking)
    {
        ai.state = AIState::Idle;
        ai.stateTimer = 0;
        ai.target = nullptr;
        ai.loseTargetTimer = ai.loseTargetCooldown;
        return;
    }

    auto& transform = enemy->getComponent<Transform3D>();
    auto& velocity = enemy->getComponent<Velocity3D>();
    if (!enemy->hasComponent<Combat>()) {return;}
    auto& combat = enemy->getComponent<Combat>();

    // make sure the target has a Transform
    if (!ai.target->hasComponent<Transform3D>()) return;
    auto& targetTransform = ai.target->getComponent<Transform3D>();

    glm::vec3 dir = targetTransform.position - transform.position;
    float dist = glm::length(dir);

    if (dist <= combat.attackRange && !animator.isAttacking) {
        ai.state = AIState::Attack;
        ai.stateTimer = 0;
        return;
    }

    // optionally, lose target if player is far away
    if (dist > ai.visionRange * 1.5f && !animator.isAttacking) {
        ai.state = AIState::Idle;
        ai.stateTimer = 0;
        ai.target = nullptr;
        return;
    }

    // move toward player
    velocity.direction = safeNormalize(dir);
    velocity.speed = 2.5f; // chase speed
}

void EnemyAISystem::UpdateIdle(Entity* enemy, AI &ai, Animator& animator, float dt) {
    auto& velocity = enemy->getComponent<Velocity3D>();

    velocity.direction = glm::vec3(0.0f);
    velocity.speed = 0;

    // after a short time start patrol
    if (ai.stateTimer > 2.0f && enemy->hasComponent<Patrol>() && !animator.isAttacking)
    {
        ai.state = AIState::Patrol;
        ai.stateTimer = 0;
    }
}

void EnemyAISystem::UpdatePatrol(Entity* enemy, AI &ai, Animator& animator, float dt) {
    if (!enemy->hasComponent<Patrol>())
        return;

    auto& patrol = enemy->getComponent<Patrol>();
    auto& transform = enemy->getComponent<Transform3D>();
    auto& velocity = enemy->getComponent<Velocity3D>();

    glm::vec3 target = patrol.movingToB ? patrol.pointB : patrol.pointA;

    glm::vec3 dir = target - transform.position;

    float dist = glm::length(dir);

    // reached patrol point
    if (dist < 5.0f)
    {
        patrol.movingToB = !patrol.movingToB;
        return;
    }

    velocity.direction = safeNormalize(dir);
    velocity.speed = 2.5f;
}

Entity* EnemyAISystem::findPlayer(const std::vector<std::unique_ptr<Entity>>& entities) {
    for (auto& e : entities) {
        if (e->hasComponent<PlayerTag>())
            return e.get();
    }
    return nullptr;
}

// float EnemyAISystem::distance(const Transform& a, const Transform& b) {
//     float dx = a.position.x - b.position.x;
//     float dy = a.position.y - b.position.y;
//     return std::sqrt(dx * dx + dy * dy);
// }

float EnemyAISystem::distance(const Transform3D& a, const Transform3D& b) {
    return glm::distance(a.position, b.position);
}

glm::vec3 EnemyAISystem::safeNormalize(const glm::vec3& v) {
    float len = glm::length(v);
    if (len > 0.0001f)
        return v / len;
    return glm::vec3(0.0f);
}
