//
// Created by adamd on 2026-02-04.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H
#include <memory>
#include <vector>

#include "../Entity.h"

// forward declaring
class World;

class CollisionSystem {
public:
    void update(World& world);
private:
    std::vector<Entity*> queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities);
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_COLLISIONSYSTEM_H