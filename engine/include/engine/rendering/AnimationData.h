#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONDATA_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONDATA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#define MAX_BONE_INFLUENCE 4

namespace engine
{
    struct BoneInfo
    {
        int Id = 0;
        glm::mat4 Offset = glm::mat4(1.0f);
    };

    struct BoneNode
    {
        std::string Name;
        int BoneId = -1;
        glm::mat4 LocalTransform = glm::mat4(1.0f);
        int ParentIndex = -1;
    };

    struct KeyPosition
    {
        glm::vec3 Position = glm::vec3(0.0f);
        float TimeStamp = 0.0f;
    };

    struct KeyRotation
    {
        glm::quat Orientation = glm::quat(1, 0, 0, 0);
        float TimeStamp = 0.0f;
    };

    struct KeyScale
    {
        glm::vec3 Scale = glm::vec3(1.0f);
        float TimeStamp = 0.0f;
    };

    struct AnimationBone
    {
        std::string Name;
        int Id = 0;
        std::vector<KeyPosition> Positions;
        std::vector<KeyRotation> Rotations;
        std::vector<KeyScale> Scales;
    };

    struct AnimationClip
    {
        std::string Name;
        float Duration = 0.0f;
        float TicksPerSecond = 25.0f;
        std::unordered_map<std::string, AnimationBone> Bones;
    };

    struct Skeleton
    {
        std::unordered_map<std::string, int> BoneMap;
        std::vector<BoneInfo> Bones;
        std::vector<BoneNode> Nodes;
        int BoneCount = 0;
        int RootNodeIndex = 0;
    };

    struct Pose
    {
        std::vector<glm::vec3> Positions;
        std::vector<glm::quat> Rotations;
        std::vector<glm::vec3> Scales;
    };
}

#endif
