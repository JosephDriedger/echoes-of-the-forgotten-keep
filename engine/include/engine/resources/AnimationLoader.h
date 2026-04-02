// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONLOADER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONLOADER_H

#include "engine/rendering/AnimationData.h"

#include <memory>
#include <string>
#include <vector>

namespace engine
{
    class AnimationLoader
    {
    public:
        std::shared_ptr<std::vector<AnimationClip>> LoadFromFile(const std::string& path) const;

        static void AppendClips(
            const std::string& path,
            std::vector<AnimationClip>& targetClips,
            const AnimationLoader& loader);
    };
}

#endif
