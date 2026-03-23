//
// Created by adamd on 2026-03-16.
//

#include "AnimationManager.h"

#include <iostream>
#include <string>
#include <vector>

#include "Animation3DSystem.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "glm/fwd.hpp"

std::unordered_map<std::string, std::vector<Animation3DClip>*> AnimationManager::animations;

std::vector<Animation3DClip>* AnimationManager::load(const std::string& path)
{
    auto it = animations.find(path);
    if(it != animations.end())
        return it->second;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_LimitBoneWeights
    );

    if(!scene || !scene->HasAnimations())
    {
        std::cout << "No animations in file\n";
        return nullptr;
    }

    auto *clips = new std::vector<Animation3DClip>();

    for(unsigned int a = 0; a < scene->mNumAnimations; a++)
    {
        aiAnimation* anim = scene->mAnimations[a];

        Animation3DClip clip;

        clip.name = anim->mName.C_Str();
        clip.duration = anim->mDuration;
        clip.ticksPerSecond = anim->mTicksPerSecond;

        for(unsigned int c = 0; c < anim->mNumChannels; c++)
        {
            aiNodeAnim* channel = anim->mChannels[c];

            AnimationBone bone;
            bone.name = channel->mNodeName.C_Str();

            // Positions
            for(unsigned int i=0;i<channel->mNumPositionKeys;i++)
            {
                KeyPosition kp;

                kp.position = glm::vec3(
                    channel->mPositionKeys[i].mValue.x,
                    channel->mPositionKeys[i].mValue.y,
                    channel->mPositionKeys[i].mValue.z
                );

                kp.timeStamp = channel->mPositionKeys[i].mTime;

                bone.positions.push_back(kp);
            }

            // Rotations
            for(unsigned int i=0;i<channel->mNumRotationKeys;i++)
            {
                KeyRotation kr{};

                kr.orientation = glm::quat(
                    channel->mRotationKeys[i].mValue.w,
                    channel->mRotationKeys[i].mValue.x,
                    channel->mRotationKeys[i].mValue.y,
                    channel->mRotationKeys[i].mValue.z
                );

                kr.timeStamp = channel->mRotationKeys[i].mTime;

                bone.rotations.push_back(kr);
            }

            // Scales
            for(unsigned int i=0;i<channel->mNumScalingKeys;i++)
            {
                KeyScale ks{};

                ks.scale = glm::vec3(
                    channel->mScalingKeys[i].mValue.x,
                    channel->mScalingKeys[i].mValue.y,
                    channel->mScalingKeys[i].mValue.z
                );

                ks.timeStamp = channel->mScalingKeys[i].mTime;

                bone.scales.push_back(ks);
            }

            clip.bones[bone.name] = bone;
        }

        clips->push_back(clip);

        std::cout << "Loaded animation: " << clip.name << "\n";
    }

    animations[path] = clips;

    return clips;
}

void AnimationManager::appendClips(
    const std::string& path,
    std::vector<Animation3DClip>& targetClips)
{
    std::vector<Animation3DClip>* newClips = load(path);

    if (!newClips)
    {
        std::cout << "Failed to load animations from: " << path << "\n";
        return;
    }

    for (auto& clip : *newClips)
    {
        targetClips.push_back(clip);

        std::cout << "Appended animation: "
                  << clip.name
                  << " from " << path << "\n";
    }
}