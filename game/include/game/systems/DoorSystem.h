// Created by Elijah Fabon

// DoorSystem -- Handles proximity-based door opening/closing.
// When the player approaches a door within trigger distance, the door
// swings open in the direction the player is approaching from. The door
// closes automatically when the player moves away.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DOORSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DOORSYSTEM_H

#include "engine/ecs/Registry.h"
#include "engine/resources/AudioEventQueue.h"

namespace game
{
    class DoorSystem
    {
    public:
        static void Update(engine::Registry& registry, float deltaTime, engine::AudioEventQueue &audioEventQueue);
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_DOORSYSTEM_H
