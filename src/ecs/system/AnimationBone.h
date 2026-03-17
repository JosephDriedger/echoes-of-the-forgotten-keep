//
// Created by adamd on 2026-03-16.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONBONE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONBONE_H
#include <string>

#include "AnimationKey.h"

struct AnimationBone
{
    std::string name;
    int id;

    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONBONE_H