// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATOR_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATOR_H

#include "engine/rendering/AnimationData.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace engine
{
    class Animator
    {
    public:
        static glm::vec3 InterpolatePosition(const std::vector<KeyPosition>& positions, float time);
        static glm::quat InterpolateRotation(const std::vector<KeyRotation>& rotations, float time);
        static glm::vec3 InterpolateScale(const std::vector<KeyScale>& scales, float time);

        static Pose SamplePose(const Skeleton& skeleton, const AnimationClip& clip, float time);
        static Pose BlendPoses(const Pose& a, const Pose& b, float t);
        static void ApplyPose(
            const Skeleton& skeleton,
            const Pose& pose,
            std::vector<glm::mat4>& finalBoneMatrices,
            std::unordered_map<std::string, glm::mat4>& finalNodeTransforms);
    };
}

#endif
