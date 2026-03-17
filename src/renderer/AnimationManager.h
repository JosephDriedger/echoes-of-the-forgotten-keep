//
// Created by adamd on 2026-03-16.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONMANAGER_H

#include "assimp/scene.h"
#include "glm/detail/type_quat.hpp"
#include "Animation3DClips.h"

class AnimationManager
{
public:

    static std::unordered_map<std::string, std::vector<Animation3DClip>*> animations;

    static std::vector<Animation3DClip>* load(const std::string& path);
    static void appendClips(const std::string& path, std::vector<Animation3DClip>& targetClips);
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONMANAGER_H