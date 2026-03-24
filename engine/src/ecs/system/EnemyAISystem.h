//
// Created by scept on 2026-03-09.
//

#ifndef INC_8051TUTORIAL_ENEMYAISYSTEM_H
#define INC_8051TUTORIAL_ENEMYAISYSTEM_H

#include <memory>
#include <vector>
#include "Entity.h"
#include "Component.h"

class EnemyAISystem
{
public:
    EnemyAISystem() = default;

    // called once per frame
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt);

private:
    std::vector<Entity*> queryEnemies(const std::vector<std::unique_ptr<Entity>>& entities);

    // state handlers
    void UpdateIdle(Entity* enemy, AI& ai, float dt);
    void UpdatePatrol(Entity* enemy, AI& ai, float dt);
    void UpdateChase(Entity* enemy, AI& ai, float dt);
    void UpdateAttack(Entity* enemy, AI& ai, float dt);

    // helper functions
    Entity* findPlayer(const std::vector<std::unique_ptr<Entity>>& entities);
    float distance(const Transform3D& a, const Transform3D& b);
    glm::vec3 safeNormalize(const glm::vec3& v);

};

#endif //INC_8051TUTORIAL_ENEMYAISYSTEM_H