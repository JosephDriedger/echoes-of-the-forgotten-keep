// Created by Joey Driedger

#include "engine/resources/AnimationLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

namespace engine
{
    std::shared_ptr<std::vector<AnimationClip>> AnimationLoader::LoadFromFile(const std::string& path) const
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_LimitBoneWeights
        );

        if (!scene || !scene->HasAnimations())
        {
            std::cerr << "[AnimationLoader] No animations in: " << path << '\n';
            return nullptr;
        }

        auto clips = std::make_shared<std::vector<AnimationClip>>();

        for (unsigned int a = 0; a < scene->mNumAnimations; a++)
        {
            aiAnimation* anim = scene->mAnimations[a];

            AnimationClip clip;
            clip.Name = anim->mName.C_Str();
            clip.Duration = static_cast<float>(anim->mDuration);
            clip.TicksPerSecond = static_cast<float>(
                anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f);

            for (unsigned int c = 0; c < anim->mNumChannels; c++)
            {
                aiNodeAnim* channel = anim->mChannels[c];

                AnimationBone bone;
                bone.Name = channel->mNodeName.C_Str();

                for (unsigned int i = 0; i < channel->mNumPositionKeys; i++)
                {
                    KeyPosition kp;
                    kp.Position = glm::vec3(
                        channel->mPositionKeys[i].mValue.x,
                        channel->mPositionKeys[i].mValue.y,
                        channel->mPositionKeys[i].mValue.z);
                    kp.TimeStamp = static_cast<float>(channel->mPositionKeys[i].mTime);
                    bone.Positions.push_back(kp);
                }

                for (unsigned int i = 0; i < channel->mNumRotationKeys; i++)
                {
                    KeyRotation kr;
                    kr.Orientation = glm::quat(
                        static_cast<float>(channel->mRotationKeys[i].mValue.w),
                        static_cast<float>(channel->mRotationKeys[i].mValue.x),
                        static_cast<float>(channel->mRotationKeys[i].mValue.y),
                        static_cast<float>(channel->mRotationKeys[i].mValue.z));
                    kr.TimeStamp = static_cast<float>(channel->mRotationKeys[i].mTime);
                    bone.Rotations.push_back(kr);
                }

                for (unsigned int i = 0; i < channel->mNumScalingKeys; i++)
                {
                    KeyScale ks;
                    ks.Scale = glm::vec3(
                        channel->mScalingKeys[i].mValue.x,
                        channel->mScalingKeys[i].mValue.y,
                        channel->mScalingKeys[i].mValue.z);
                    ks.TimeStamp = static_cast<float>(channel->mScalingKeys[i].mTime);
                    bone.Scales.push_back(ks);
                }

                clip.Bones[bone.Name] = bone;
            }

            clips->push_back(clip);
        }

        return clips;
    }

    void AnimationLoader::AppendClips(
        const std::string& path,
        std::vector<AnimationClip>& targetClips,
        const AnimationLoader& loader)
    {
        auto newClips = loader.LoadFromFile(path);
        if (!newClips)
        {
            std::cerr << "[AnimationLoader] Failed to append from: " << path << '\n';
            return;
        }

        for (auto& clip : *newClips)
        {
            targetClips.push_back(clip);
        }
    }
}
