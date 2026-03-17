//
// Created by adamd on 2026-03-16.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_BONENODE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_BONENODE_H
#include <string>
#include <vector>

#include "glm/fwd.hpp"

struct BoneNode {
    std::string name;
    int boneID = -1;             // index in boneInfo array (-1 if not a bone)
    glm::mat4 localTransform;    // node->mTransformation
    std::vector<int> children;   // indices into a flat array
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_BONENODE_H