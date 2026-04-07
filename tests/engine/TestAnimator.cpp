//
// Tests for the skeletal animation Animator: keyframe interpolation (position,
// rotation, scale), pose sampling, pose blending, and final bone matrix computation.
//

#include "engine/rendering/Animator.h"
#include "engine/rendering/AnimationData.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace
{
    // Floating-point comparison helpers for animation value verification
    bool NearEqual(float a, float b, float epsilon = 0.001f)
    {
        return std::fabs(a - b) < epsilon;
    }

    bool Vec3Near(const glm::vec3& a, const glm::vec3& b, float epsilon = 0.001f)
    {
        return NearEqual(a.x, b.x, epsilon) &&
               NearEqual(a.y, b.y, epsilon) &&
               NearEqual(a.z, b.z, epsilon);
    }

    // Build a minimal skeleton with a root and one child bone
    engine::Skeleton MakeTestSkeleton()
    {
        engine::Skeleton skeleton;

        // Root node (no bone)
        engine::BoneNode root;
        root.Name = "Root";
        root.BoneId = -1;
        root.ParentIndex = -1;
        skeleton.Nodes.push_back(root);

        // Child bone
        engine::BoneNode bone0;
        bone0.Name = "Bone0";
        bone0.BoneId = 0;
        bone0.ParentIndex = 0;
        skeleton.Nodes.push_back(bone0);

        // Second child bone
        engine::BoneNode bone1;
        bone1.Name = "Bone1";
        bone1.BoneId = 1;
        bone1.ParentIndex = 0;
        skeleton.Nodes.push_back(bone1);

        skeleton.BoneMap["Root"] = -1;
        skeleton.BoneMap["Bone0"] = 0;
        skeleton.BoneMap["Bone1"] = 1;

        engine::BoneInfo info0;
        info0.Id = 0;
        info0.Offset = glm::mat4(1.0f);

        engine::BoneInfo info1;
        info1.Id = 1;
        info1.Offset = glm::mat4(1.0f);

        skeleton.Bones.push_back(info0);
        skeleton.Bones.push_back(info1);
        skeleton.BoneCount = 2;
        skeleton.RootNodeIndex = 0;

        return skeleton;
    }

    // Build a simple animation clip with 2 keyframes per bone
    engine::AnimationClip MakeTestClip()
    {
        engine::AnimationClip clip;
        clip.Name = "TestClip";
        clip.Duration = 10.0f;
        clip.TicksPerSecond = 1.0f;

        // Root has no animation (will use defaults)

        // Bone0: moves from (0,0,0) to (10,0,0)
        engine::AnimationBone bone0;
        bone0.Name = "Bone0";
        bone0.Id = 0;
        bone0.Positions = {
            {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f},
            {glm::vec3(10.0f, 0.0f, 0.0f), 10.0f}
        };
        bone0.Rotations = {
            {glm::quat(1, 0, 0, 0), 0.0f},
            {glm::quat(1, 0, 0, 0), 10.0f}
        };
        bone0.Scales = {
            {glm::vec3(1.0f), 0.0f},
            {glm::vec3(1.0f), 10.0f}
        };
        clip.Bones["Bone0"] = bone0;

        // Bone1: moves from (0,0,0) to (0,5,0)
        engine::AnimationBone bone1;
        bone1.Name = "Bone1";
        bone1.Id = 1;
        bone1.Positions = {
            {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f},
            {glm::vec3(0.0f, 5.0f, 0.0f), 10.0f}
        };
        bone1.Rotations = {
            {glm::quat(1, 0, 0, 0), 0.0f},
            {glm::quat(1, 0, 0, 0), 10.0f}
        };
        bone1.Scales = {
            {glm::vec3(1.0f), 0.0f},
            {glm::vec3(2.0f), 10.0f}
        };
        clip.Bones["Bone1"] = bone1;

        return clip;
    }
}

int RunAnimatorTests()
{
    // Test position interpolation at midpoint, start, and end of two keyframes
    {
        std::vector<engine::KeyPosition> positions = {
            {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f},
            {glm::vec3(10.0f, 0.0f, 0.0f), 10.0f}
        };

        glm::vec3 mid = engine::Animator::InterpolatePosition(positions, 5.0f);
        assert(Vec3Near(mid, glm::vec3(5.0f, 0.0f, 0.0f)));

        glm::vec3 start = engine::Animator::InterpolatePosition(positions, 0.0f);
        assert(Vec3Near(start, glm::vec3(0.0f, 0.0f, 0.0f)));

        glm::vec3 end = engine::Animator::InterpolatePosition(positions, 10.0f);
        assert(Vec3Near(end, glm::vec3(10.0f, 0.0f, 0.0f)));
    }

    // Test that a single keyframe returns its value regardless of query time
    {
        std::vector<engine::KeyPosition> positions = {
            {glm::vec3(3.0f, 4.0f, 5.0f), 0.0f}
        };
        glm::vec3 result = engine::Animator::InterpolatePosition(positions, 5.0f);
        assert(Vec3Near(result, glm::vec3(3.0f, 4.0f, 5.0f)));
    }

    // Test that empty keyframes return zero vector (safe default)
    {
        std::vector<engine::KeyPosition> positions;
        glm::vec3 result = engine::Animator::InterpolatePosition(positions, 5.0f);
        assert(Vec3Near(result, glm::vec3(0.0f)));
    }

    // Test quaternion slerp interpolation between identity and 90-degree Y rotation
    {
        glm::quat q0 = glm::quat(1, 0, 0, 0); // identity
        glm::quat q1 = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));

        std::vector<engine::KeyRotation> rotations = {
            {q0, 0.0f},
            {q1, 10.0f}
        };

        glm::quat mid = engine::Animator::InterpolateRotation(rotations, 5.0f);
        // At midpoint, should be ~45 degree rotation around Y
        glm::quat expected = glm::normalize(glm::slerp(q0, q1, 0.5f));
        // Verify quaternions are equivalent (dot product near 1.0)
        float dot = std::fabs(glm::dot(mid, expected));
        assert(dot > 0.999f);
    }

    // Test scale interpolation: (1,1,1) to (3,3,3) at midpoint should yield (2,2,2)
    {
        std::vector<engine::KeyScale> scales = {
            {glm::vec3(1.0f), 0.0f},
            {glm::vec3(3.0f), 10.0f}
        };

        glm::vec3 mid = engine::Animator::InterpolateScale(scales, 5.0f);
        assert(Vec3Near(mid, glm::vec3(2.0f)));
    }

    // Test SamplePose: samples all bone channels at a given time across the skeleton
    {
        engine::Skeleton skeleton = MakeTestSkeleton();
        engine::AnimationClip clip = MakeTestClip();

        engine::Pose pose = engine::Animator::SamplePose(skeleton, clip, 5.0f);

        // Pose should contain one entry per skeleton node (Root + Bone0 + Bone1)
        assert(pose.Positions.size() == 3);
        assert(pose.Rotations.size() == 3);
        assert(pose.Scales.size() == 3);

        // Verify per-bone interpolated values at the midpoint of the clip
        assert(Vec3Near(pose.Positions[1], glm::vec3(5.0f, 0.0f, 0.0f)));
        assert(Vec3Near(pose.Positions[2], glm::vec3(0.0f, 2.5f, 0.0f)));
        assert(Vec3Near(pose.Scales[2], glm::vec3(1.5f)));
    }

    // Test BlendPoses at 50% blend factor between two poses
    {
        engine::Pose poseA;
        poseA.Positions = {glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 0.0f)};
        poseA.Rotations = {glm::quat(1, 0, 0, 0), glm::quat(1, 0, 0, 0)};
        poseA.Scales = {glm::vec3(1.0f), glm::vec3(1.0f)};

        engine::Pose poseB;
        poseB.Positions = {glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f)};
        poseB.Rotations = {glm::quat(1, 0, 0, 0), glm::quat(1, 0, 0, 0)};
        poseB.Scales = {glm::vec3(1.0f), glm::vec3(2.0f)};

        engine::Pose blended = engine::Animator::BlendPoses(poseA, poseB, 0.5f);

        // Position and scale should be halfway between poseA and poseB
        assert(Vec3Near(blended.Positions[1], glm::vec3(5.0f, 0.0f, 0.0f)));
        assert(Vec3Near(blended.Scales[1], glm::vec3(1.5f)));
    }

    // Test BlendPoses at extremes: factor 0.0 yields poseA, factor 1.0 yields poseB
    {
        engine::Pose poseA;
        poseA.Positions = {glm::vec3(1.0f, 2.0f, 3.0f)};
        poseA.Rotations = {glm::quat(1, 0, 0, 0)};
        poseA.Scales = {glm::vec3(1.0f)};

        engine::Pose poseB;
        poseB.Positions = {glm::vec3(4.0f, 5.0f, 6.0f)};
        poseB.Rotations = {glm::quat(1, 0, 0, 0)};
        poseB.Scales = {glm::vec3(2.0f)};

        engine::Pose at0 = engine::Animator::BlendPoses(poseA, poseB, 0.0f);
        assert(Vec3Near(at0.Positions[0], glm::vec3(1.0f, 2.0f, 3.0f)));

        engine::Pose at1 = engine::Animator::BlendPoses(poseA, poseB, 1.0f);
        assert(Vec3Near(at1.Positions[0], glm::vec3(4.0f, 5.0f, 6.0f)));
    }

    // Test ApplyPose: converts a sampled pose into final bone matrices and node transforms
    {
        engine::Skeleton skeleton = MakeTestSkeleton();
        engine::AnimationClip clip = MakeTestClip();
        engine::Pose pose = engine::Animator::SamplePose(skeleton, clip, 5.0f);

        std::vector<glm::mat4> finalBoneMatrices(100, glm::mat4(1.0f));
        std::unordered_map<std::string, glm::mat4> finalNodeTransforms;

        engine::Animator::ApplyPose(skeleton, pose, finalBoneMatrices, finalNodeTransforms);

        // Verify every skeleton node received a world-space transform
        assert(finalNodeTransforms.count("Root") == 1);
        assert(finalNodeTransforms.count("Bone0") == 1);
        assert(finalNodeTransforms.count("Bone1") == 1);

        // Bone0 translates to (5,0,0) with identity offset, so the final
        // bone matrix translation column should reflect that position
        glm::vec4 bone0Col3 = finalBoneMatrices[0][3];
        assert(NearEqual(bone0Col3.x, 5.0f));
    }

    std::cout << "Animator tests passed.\n";
    return 0;
}
