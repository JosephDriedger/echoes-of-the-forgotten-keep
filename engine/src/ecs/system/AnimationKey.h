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

struct AnimationBone
{
    std::string name;
    int id;

    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;
};

struct BoneNode {
    std::string name;
    int boneID = -1;             // index in boneInfo array (-1 if not a bone)
    glm::mat4 localTransform;    // node->mTransformation

    int parentIndex = -1;

    //std::vector<int> children;   // indices into a flat array
};

struct Animation3DClip {
    std::string name;
    float duration = 0.0f;
    float ticksPerSecond = 25.0f;
    std::unordered_map<std::string, AnimationBone> bones;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONKEY_H