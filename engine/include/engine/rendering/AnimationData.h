// Created by Adam Van Woerden
// Skeletal animation data structures used by the Animator and model loader.
// Defines the bone hierarchy, keyframe data, animation clips, and sampled poses
// that drive GPU skinning in the vertex shader.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONDATA_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONDATA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <unordered_map>
#include <vector>

// Maximum number of bones that can influence a single vertex (matches Vertex layout).
#define MAX_BONE_INFLUENCE 4

namespace engine
{
    // Maps a bone to its index and its inverse bind-pose (offset) matrix.
    // The offset matrix transforms vertices from model space to bone-local space.
    struct BoneInfo
    {
        int Id = 0;
        glm::mat4 Offset = glm::mat4(1.0f);
    };

    // A node in the skeleton hierarchy. Not all nodes are bones (some are
    // intermediate transform nodes from the scene graph). BoneId is -1 if
    // this node does not deform any vertices.
    struct BoneNode
    {
        std::string Name;
        int BoneId = -1;
        glm::mat4 LocalTransform = glm::mat4(1.0f);
        int ParentIndex = -1;  // Index into Skeleton::Nodes; -1 for root
    };

    // Keyframe types for position, rotation, and scale channels.
    struct KeyPosition
    {
        glm::vec3 Position = glm::vec3(0.0f);
        float TimeStamp = 0.0f;
    };

    struct KeyRotation
    {
        glm::quat Orientation = glm::quat(1, 0, 0, 0);  // Identity quaternion (w,x,y,z)
        float TimeStamp = 0.0f;
    };

    struct KeyScale
    {
        glm::vec3 Scale = glm::vec3(1.0f);
        float TimeStamp = 0.0f;
    };

    // Per-bone keyframe tracks within an animation clip.
    struct AnimationBone
    {
        std::string Name;
        int Id = 0;
        std::vector<KeyPosition> Positions;
        std::vector<KeyRotation> Rotations;
        std::vector<KeyScale> Scales;
    };

    // A single animation sequence (e.g., "walk", "idle") with duration and
    // per-bone keyframe channels. Time values are in ticks.
    struct AnimationClip
    {
        std::string Name;
        float Duration = 0.0f;
        float TicksPerSecond = 25.0f;
        std::unordered_map<std::string, AnimationBone> Bones;
    };

    // The complete skeleton: bone metadata, the flat node hierarchy, and a
    // name-to-index lookup map. Nodes are stored in parent-before-child order
    // so a single forward pass can compute global transforms.
    struct Skeleton
    {
        std::unordered_map<std::string, int> BoneMap;  // Bone name -> bone index
        std::vector<BoneInfo> Bones;
        std::vector<BoneNode> Nodes;
        int BoneCount = 0;
        int RootNodeIndex = 0;
    };

    // A sampled animation pose: one transform per skeleton node.
    // Used as an intermediate result between sampling and applying to the skeleton.
    struct Pose
    {
        std::vector<glm::vec3> Positions;
        std::vector<glm::quat> Rotations;
        std::vector<glm::vec3> Scales;
    };
}

#endif
