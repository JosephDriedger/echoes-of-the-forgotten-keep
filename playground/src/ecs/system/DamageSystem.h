//
// Created by scept on 2026-03-09.
//

#ifndef INC_8051TUTORIAL_DAMAGESYSTEM_H
#define INC_8051TUTORIAL_DAMAGESYSTEM_H

#include <vector>
#include <memory>

class Entity;

class DamageSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities);
};

#endif //INC_8051TUTORIAL_DAMAGESYSTEM_H