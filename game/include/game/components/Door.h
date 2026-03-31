#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_DOOR_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_DOOR_H

#include <string>

namespace game
{
    struct Door
    {
        float BaseRotationY = 0.0f;   // original rotation when spawned
        float CurrentAngle = 0.0f;    // current swing angle in degrees (0 = closed)
        float SwingSpeed = 180.0f;    // degrees per second
        int SwingDirection = 0;       // +1 or -1, determined by player approach side

        // Trigger-based door puzzle fields
        std::string TriggerId;        // links to a Switch::Id
        bool Open = false;
        float TargetAngle = 0.0f;
        float OpenAngle = 90.0f;      // degrees when fully open
        float CloseAngle = 0.0f;

        Door() = default;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_DOOR_H
