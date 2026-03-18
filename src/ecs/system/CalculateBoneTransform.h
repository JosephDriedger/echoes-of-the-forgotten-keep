//
// Created by adamd on 2026-03-16.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_CALCULATEBONETRANSFORM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_CALCULATEBONETRANSFORM_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "AnimationBone.h"
#include "BoneNode.h"
#include "Component.h"

namespace CalculateBoneTransform {

inline glm::vec3 InterpolatePosition(const std::vector<KeyPosition>& positions, float time)
{
    if (positions.empty()) return glm::vec3(0.0f);

    if (positions.size() == 1) return positions[0].position;

    int i = 0;
    for (; i < positions.size() - 1; i++)
    {
        if (time < positions[i + 1].timeStamp)
            break;
    }

    std::vector<KeyPosition>::value_type p0 = positions[i];
    std::vector<KeyPosition>::value_type p1 = positions[i + 1];

    float factor = (time - p0.timeStamp) / (p1.timeStamp - p0.timeStamp);
    return glm::mix(p0.position, p1.position, factor);
}

inline glm::quat InterpolateRotation(const std::vector<KeyRotation>& rotations, float time)
{
    if (rotations.empty()) return glm::quat(1, 0, 0, 0);
    if (rotations.size() == 1) return rotations[0].orientation;

    int i = 0;
    for (; i < rotations.size() - 1; i++)
    {
        if (time < rotations[i + 1].timeStamp)
            break;
    }

    std::vector<KeyRotation>::value_type r0 = rotations[i];
    std::vector<KeyRotation>::value_type r1 = rotations[i + 1];

    float factor = (time - r0.timeStamp) / (r1.timeStamp - r0.timeStamp);
    return glm::slerp(r0.orientation, r1.orientation, factor);
}

inline glm::vec3 InterpolateScale(const std::vector<KeyScale>& scales, float time)
{
    if (scales.empty()) return glm::vec3(1.0f);
    if (scales.size() == 1) return scales[0].scale;

    int i = 0;
    for (; i < scales.size() - 1; i++)
    {
        if (time < scales[i + 1].timeStamp)
            break;
    }

    std::vector<KeyScale>::value_type s0 = scales[i];
    std::vector<KeyScale>::value_type s1 = scales[i + 1];

    float factor = (time - s0.timeStamp) / (s1.timeStamp - s0.timeStamp);
    return glm::mix(s0.scale, s1.scale, factor);
}

// Recursive calculation
inline void Calculate(
    BoneNode& node,
    const glm::mat4& parentTransform,
    Model& model,
    const Animation3DClip& clip,
    Animator& animator
)
{
    glm::mat4 nodeTransform = node.localTransform;

    // Find animation for this bone
    for (const auto& animBone : clip.bones)
    {
        if (animBone.first == node.name)
        {
            glm::vec3 pos = InterpolatePosition(animBone.second.positions, animator.currentTime);
            glm::quat rot = InterpolateRotation(animBone.second.rotations, animator.currentTime);
            glm::vec3 scale = InterpolateScale(animBone.second.scales, animator.currentTime);

            nodeTransform = glm::translate(glm::mat4(1.0f), pos)
                          * glm::toMat4(rot)
                          * glm::scale(glm::mat4(1.0f), scale);
            break;
        }
    }

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    animator.finalNodeTransforms[node.name] = globalTransform;

    if (node.boneID >= 0 && node.boneID < animator.finalBoneMatrices.size())
    {
        // multiply by offset if you stored it in Model.boneInfo
        auto offset = glm::mat4(1.0f);
        if (node.boneID < model.boneInfo.size())
            offset = model.boneInfo[node.boneID].offset;

        animator.finalBoneMatrices[node.boneID] = globalTransform * offset;
    }

    // recurse children
    for (int childIdx : node.children)
    {
        if (childIdx >= 0 && childIdx < model.skeleton.size())
            Calculate(model.skeleton[childIdx], globalTransform, model, clip, animator);
    }


}

} // namespace CalculateBoneTransform

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_CALCULATEBONETRANSFORM_H