//
// Created by adamd on 2026-03-13.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_BONE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_BONE_H

#include <glm/glm.hpp>
#include <string>

struct BoneInfo
{
    int id;
    glm::mat4 offset;
    int finalTransform;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_BONE_H