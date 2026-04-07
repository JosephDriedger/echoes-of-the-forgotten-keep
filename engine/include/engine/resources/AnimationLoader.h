// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONLOADER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONLOADER_H

#include "engine/rendering/AnimationData.h"

#include <memory>
#include <string>
#include <vector>

namespace engine
{
    // Imports skeletal animation clips from model files via Assimp.
    // Each file may contain multiple clips with per-bone keyframe channels.
    class AnimationLoader
    {
    public:
        // Parses all animation clips from a model file.
        // Returns nullptr if the file has no animations.
        std::shared_ptr<std::vector<AnimationClip>> LoadFromFile(const std::string& path) const;

        // Convenience: loads clips from a file and appends them to an existing collection.
        static void AppendClips(
            const std::string& path,
            std::vector<AnimationClip>& targetClips,
            const AnimationLoader& loader);
    };
}

#endif
