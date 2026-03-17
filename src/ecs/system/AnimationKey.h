//
// Created by adamd on 2026-03-16.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONKEY_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONKEY_H
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"
#include "glm/detail/type_quat.hpp"

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONKEY_H