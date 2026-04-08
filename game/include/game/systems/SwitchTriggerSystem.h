// Created by Elijah Fabon

// SwitchTriggerSystem -- Detects AABB overlap between the player and
// floor switch entities. Sets each Switch's Pressed flag to true while
// the player stands on it, and resets it when they step off. Feeds
// state into DoorPuzzleSystem for trigger-linked doors.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SWITCHTRIGGERSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SWITCHTRIGGERSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class SwitchTriggerSystem
    {
    public:
        static void Update(engine::Registry& registry);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SWITCHTRIGGERSYSTEM_H
