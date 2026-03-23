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

    float delta = p1.timeStamp - p0.timeStamp;

    if (fabs(delta) < 0.0001f)
        return p0.position;

    float factor = (time - p0.timeStamp) / delta;
    factor = glm::clamp(factor, 0.0f, 1.0f);

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

    float delta = r1.timeStamp - r0.timeStamp;

    if (fabs(delta) < 0.0001f)
        return r0.orientation;

    float factor = (time - r0.timeStamp) / delta;

    factor = glm::clamp(factor, 0.0f, 1.0f);
    // float factor = (time - r0.timeStamp) / (r1.timeStamp - r0.timeStamp);
    return glm::normalize(glm::slerp(r0.orientation, r1.orientation, factor));
    // return glm::slerp(r0.orientation, r1.orientation, factor);
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

    float delta = s1.timeStamp - s0.timeStamp;

    if (fabs(delta) < 0.0001f)
        return s0.scale;

    float factor = (time - s0.timeStamp) / delta;
    factor = glm::clamp(factor, 0.0f, 1.0f);

    // float factor = (time - s0.timeStamp) / (s1.timeStamp - s0.timeStamp);
    return glm::mix(s0.scale, s1.scale, factor);
}

// inline void Calculate(
//         Model& model,
//         const Animation3DClip& clip,
//         Animator& animator
//     )
// {
//     std::vector<glm::mat4> globalTransforms(model.skeleton.size());
//
//     for (int i = 0; i < model.skeleton.size(); i++)
//     {
//         BoneNode& node = model.skeleton[i];
//
//         glm::mat4 nodeTransform = node.localTransform;
//
//         // 🔥 animation lookup
//         auto it = clip.bones.find(node.name);
//         if (it != clip.bones.end())
//         {
//             const auto& animBone = it->second;
//
//             glm::vec3 pos = InterpolatePosition(animBone.positions, animator.currentTime);
//             glm::quat rot = InterpolateRotation(animBone.rotations, animator.currentTime);
//             glm::vec3 scale = InterpolateScale(animBone.scales, animator.currentTime);
//
//             nodeTransform =
//                 glm::translate(glm::mat4(1.0f), pos) *
//                 glm::toMat4(rot) *
//                 glm::scale(glm::mat4(1.0f), scale);
//         }
//
//         // 🔥 parent transform
//         glm::mat4 parentTransform = glm::mat4(1.0f);
//
//         if (node.parentIndex != -1)
//             parentTransform = globalTransforms[node.parentIndex];
//
//         glm::mat4 globalTransform = parentTransform * nodeTransform;
//
//         globalTransforms[i] = globalTransform;
//
//         // 🔥 store for attachments
//         animator.finalNodeTransforms[node.name] = globalTransform;
//
//         // 🔥 final bone matrix
//         if (node.boneID >= 0 && node.boneID < animator.finalBoneMatrices.size())
//         {
//             glm::mat4 offset = glm::mat4(1.0f);
//             if (node.boneID < model.boneInfo.size())
//                 offset = model.boneInfo[node.boneID].offset;
//
//             animator.finalBoneMatrices[node.boneID] = globalTransform * offset;
//         }
//     }
// }

inline Pose SamplePose(Model& model, const Animation3DClip& clip, float time)
{
    Pose pose;
    pose.positions.resize(model.skeleton.size());
    pose.rotations.resize(model.skeleton.size());
    pose.scales.resize(model.skeleton.size());

    for (int i = 0; i < model.skeleton.size(); i++)
    {
        BoneNode& node = model.skeleton[i];

        glm::vec3 pos(0.0f);
        glm::quat rot(1, 0, 0, 0);
        glm::vec3 scale(1.0f);

        auto it = clip.bones.find(node.name);
        if (it != clip.bones.end())
        {
            const auto& animBone = it->second;

            pos = InterpolatePosition(animBone.positions, time);
            rot = InterpolateRotation(animBone.rotations, time);
            scale = InterpolateScale(animBone.scales, time);
        }

        pose.positions[i] = pos;
        pose.rotations[i] = rot;
        pose.scales[i] = scale;
    }

    return pose;
}

inline Pose BlendPoses(const Pose& a, const Pose& b, float t)
{
    Pose result;
    result.positions.resize(a.positions.size());
    result.rotations.resize(a.rotations.size());
    result.scales.resize(a.scales.size());

    for (int i = 0; i < a.positions.size(); i++)
    {
        result.positions[i] = glm::mix(a.positions[i], b.positions[i], t);
        result.rotations[i] = glm::normalize(glm::slerp(a.rotations[i], b.rotations[i], t));
        result.scales[i] = glm::mix(a.scales[i], b.scales[i], t);
    }

    return result;
}

inline void ApplyPose(
        Model& model,
        const Pose& pose,
        Animator& animator
    )
{
    std::vector<glm::mat4> globalTransforms(model.skeleton.size());

    for (int i = 0; i < model.skeleton.size(); i++)
    {
        BoneNode& node = model.skeleton[i];

        glm::mat4 nodeTransform =
            glm::translate(glm::mat4(1.0f), pose.positions[i]) *
            glm::toMat4(pose.rotations[i]) *
            glm::scale(glm::mat4(1.0f), pose.scales[i]);

        glm::mat4 parentTransform = glm::mat4(1.0f);

        if (node.parentIndex != -1)
            parentTransform = globalTransforms[node.parentIndex];

        glm::mat4 global = parentTransform * nodeTransform;

        globalTransforms[i] = global;

        animator.finalNodeTransforms[node.name] = global;

        if (node.boneID >= 0 && node.boneID < animator.finalBoneMatrices.size())
        {
            glm::mat4 offset = glm::mat4(1.0f);
            if (node.boneID < model.boneInfo.size())
                offset = model.boneInfo[node.boneID].offset;

            animator.finalBoneMatrices[node.boneID] = global * offset;
        }
    }
}

} // namespace CalculateBoneTransform

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_CALCULATEBONETRANSFORM_H