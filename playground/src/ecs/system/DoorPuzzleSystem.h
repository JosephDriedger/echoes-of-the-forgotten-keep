//
// Created by scept on 2026-03-29.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H
#include <memory>
#include <vector>

#include "Entity.h"

class DoorPuzzleSystem {
    public:
    void update(std::vector<std::unique_ptr<Entity> > &entities);
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H