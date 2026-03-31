//
// Created by adamd on 2026-03-31.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_AI_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_AI_H
#include "engine/ecs/Entity.h"
#include "glm/vec3.hpp"

enum class AIState {
    Idle,
    Patrol,
    Chase,
    Attack
};

struct Attack {
    float cooldown = 3.0f;
    float timer = 0.0f;
};

struct AttackRequest {
    // Vector2D direction{};
    glm::vec3 direction{};
    std::string tag;
};

struct Combat {
    Attack attack;
    float attackRange = 1.0f;
    engine::Entity* weapon = nullptr;
};

struct Damage {
    int amount = 1;
};

struct AI {
    AIState state = AIState::Idle;
    float stateTimer = 0.0f;

    float visionRange = 200.0f;

    engine::Entity* target = nullptr;

    float maxChaseTime = 5.0f;
    float loseTargetCooldown = 2.0f;
    float loseTargetTimer = 0.0f;

    Combat combat;
    Attack attack;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_AI_H