#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_DOOR_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_DOOR_H

#include "engine/ecs/Entity.h"
#include <string>

namespace game
{
    struct Door
    {
        float BaseRotationY = 0.0f;   // original rotation when spawned
        float CurrentAngle = 0.0f;    // current swing angle in degrees (0 = closed)
        float SwingSpeed = 180.0f;    // degrees per second
        int SwingDirection = 0;       // +1 or -1, determined by player approach side

        // Trigger/puzzle-based door fields
        std::string TriggerId;        // links to Switch::Id; empty = proximity door
        bool Open = false;
        float TargetAngle = 0.0f;
        float OpenAngle = 90.0f;
        float CloseAngle = 0.0f;

        // Physical collider entity that moves with the door
        engine::Entity ColliderEntity{0};
        float PanelLength = 1.6f;     // width of the door panel
        float PanelThickness = 0.2f;  // thickness of the door panel

        Door() = default;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_DOOR_H
