#include "engine/rendering/Animator.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>

namespace engine
{
    glm::vec3 Animator::InterpolatePosition(const std::vector<KeyPosition>& positions, float time)
    {
        if (positions.empty()) return glm::vec3(0.0f);
        if (positions.size() == 1) return positions[0].Position;

        int i = 0;
        for (; i < static_cast<int>(positions.size()) - 1; i++)
        {
            if (time < positions[i + 1].TimeStamp)
                break;
        }

        const KeyPosition& p0 = positions[i];
        const KeyPosition& p1 = positions[i + 1];

        float delta = p1.TimeStamp - p0.TimeStamp;
        if (std::fabs(delta) < 0.0001f)
            return p0.Position;

        float factor = glm::clamp((time - p0.TimeStamp) / delta, 0.0f, 1.0f);
        return glm::mix(p0.Position, p1.Position, factor);
    }

    glm::quat Animator::InterpolateRotation(const std::vector<KeyRotation>& rotations, float time)
    {
        if (rotations.empty()) return glm::quat(1, 0, 0, 0);
        if (rotations.size() == 1) return rotations[0].Orientation;

        int i = 0;
        for (; i < static_cast<int>(rotations.size()) - 1; i++)
        {
            if (time < rotations[i + 1].TimeStamp)
                break;
        }

        const KeyRotation& r0 = rotations[i];
        const KeyRotation& r1 = rotations[i + 1];

        float delta = r1.TimeStamp - r0.TimeStamp;
        if (std::fabs(delta) < 0.0001f)
            return r0.Orientation;

        float factor = glm::clamp((time - r0.TimeStamp) / delta, 0.0f, 1.0f);
        return glm::normalize(glm::slerp(r0.Orientation, r1.Orientation, factor));
    }

    glm::vec3 Animator::InterpolateScale(const std::vector<KeyScale>& scales, float time)
    {
        if (scales.empty()) return glm::vec3(1.0f);
        if (scales.size() == 1) return scales[0].Scale;

        int i = 0;
        for (; i < static_cast<int>(scales.size()) - 1; i++)
        {
            if (time < scales[i + 1].TimeStamp)
                break;
        }

        const KeyScale& s0 = scales[i];
        const KeyScale& s1 = scales[i + 1];

        float delta = s1.TimeStamp - s0.TimeStamp;
        if (std::fabs(delta) < 0.0001f)
            return s0.Scale;

        float factor = glm::clamp((time - s0.TimeStamp) / delta, 0.0f, 1.0f);
        return glm::mix(s0.Scale, s1.Scale, factor);
    }

    Pose Animator::SamplePose(const Skeleton& skeleton, const AnimationClip& clip, float time)
    {
        Pose pose;
        pose.Positions.resize(skeleton.Nodes.size());
        pose.Rotations.resize(skeleton.Nodes.size());
        pose.Scales.resize(skeleton.Nodes.size());

        for (int i = 0; i < static_cast<int>(skeleton.Nodes.size()); i++)
        {
            const BoneNode& node = skeleton.Nodes[i];

            glm::vec3 pos(0.0f);
            glm::quat rot(1, 0, 0, 0);
            glm::vec3 scale(1.0f);

            auto it = clip.Bones.find(node.Name);
            if (it != clip.Bones.end())
            {
                const AnimationBone& animBone = it->second;
                pos = InterpolatePosition(animBone.Positions, time);
                rot = InterpolateRotation(animBone.Rotations, time);
                scale = InterpolateScale(animBone.Scales, time);
            }

            pose.Positions[i] = pos;
            pose.Rotations[i] = rot;
            pose.Scales[i] = scale;
        }

        return pose;
    }

    Pose Animator::BlendPoses(const Pose& a, const Pose& b, float t)
    {
        Pose result;
        result.Positions.resize(a.Positions.size());
        result.Rotations.resize(a.Rotations.size());
        result.Scales.resize(a.Scales.size());

        for (int i = 0; i < static_cast<int>(a.Positions.size()); i++)
        {
            result.Positions[i] = glm::mix(a.Positions[i], b.Positions[i], t);
            result.Rotations[i] = glm::normalize(glm::slerp(a.Rotations[i], b.Rotations[i], t));
            result.Scales[i] = glm::mix(a.Scales[i], b.Scales[i], t);
        }

        return result;
    }

    void Animator::ApplyPose(
        const Skeleton& skeleton,
        const Pose& pose,
        std::vector<glm::mat4>& finalBoneMatrices,
        std::unordered_map<std::string, glm::mat4>& finalNodeTransforms)
    {
        std::vector<glm::mat4> globalTransforms(skeleton.Nodes.size());

        for (int i = 0; i < static_cast<int>(skeleton.Nodes.size()); i++)
        {
            const BoneNode& node = skeleton.Nodes[i];

            glm::mat4 nodeTransform =
                glm::translate(glm::mat4(1.0f), pose.Positions[i]) *
                glm::toMat4(pose.Rotations[i]) *
                glm::scale(glm::mat4(1.0f), pose.Scales[i]);

            glm::mat4 parentTransform = glm::mat4(1.0f);
            if (node.ParentIndex != -1)
            {
                parentTransform = globalTransforms[node.ParentIndex];
            }

            glm::mat4 global = parentTransform * nodeTransform;
            globalTransforms[i] = global;

            finalNodeTransforms[node.Name] = global;

            if (node.BoneId >= 0 && node.BoneId < static_cast<int>(finalBoneMatrices.size()))
            {
                glm::mat4 offset = glm::mat4(1.0f);
                if (node.BoneId < static_cast<int>(skeleton.Bones.size()))
                {
                    offset = skeleton.Bones[node.BoneId].Offset;
                }

                finalBoneMatrices[node.BoneId] = global * offset;
            }
        }
    }
}
