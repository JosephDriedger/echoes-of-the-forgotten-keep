//
// Created by scept on 2026-03-29.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H

#pragma once

#include "engine/ecs/Registry.h"

namespace game
{
    class DoorPuzzleSystem
    {
    public:
        static void Update(engine::Registry& registry, float dt);
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_DOORPUZZLESYSTEM_H