// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATOR_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATOR_H

#include "engine/rendering/AnimationData.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace engine
{
    // Stateless utility class for skeletal animation evaluation.
    // Samples keyframes, interpolates transforms, blends poses, and produces
    // the final bone matrices that are uploaded to the vertex shader for GPU skinning.
    class Animator
    {
    public:
        // Keyframe interpolation: finds the two surrounding keyframes for the
        // given time and linearly interpolates (slerp for rotations).
        static glm::vec3 InterpolatePosition(const std::vector<KeyPosition>& positions, float time);
        static glm::quat InterpolateRotation(const std::vector<KeyRotation>& rotations, float time);
        static glm::vec3 InterpolateScale(const std::vector<KeyScale>& scales, float time);

        // Evaluates all bone channels in the clip at the given time to produce a Pose.
        static Pose SamplePose(const Skeleton& skeleton, const AnimationClip& clip, float time);

        // Linearly blends two poses by factor t (0 = pose a, 1 = pose b).
        // Uses slerp for rotation channels.
        static Pose BlendPoses(const Pose& a, const Pose& b, float t);

        // Walks the skeleton hierarchy, computes global transforms from the pose,
        // and multiplies by the inverse bind-pose offset to produce final bone
        // matrices ready for the "finalBonesMatrices[]" uniform array in the shader.
        static void ApplyPose(
            const Skeleton& skeleton,
            const Pose& pose,
            std::vector<glm::mat4>& finalBoneMatrices,
            std::unordered_map<std::string, glm::mat4>& finalNodeTransforms);
    };
}

#endif
