// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_BONEATTACHMENTSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_BONEATTACHMENTSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    /// Attaches child entities (e.g. weapons) to animated skeleton bones.
    /// Reads the parent's final bone transforms from AnimationState, combines them
    /// with the parent's world matrix and a local offset, and writes the result
    /// into the child's Transform.ModelMatrix for rendering.
    class BoneAttachmentSystem
    {
    public:
        static void Update(engine::Registry& registry);
    };
}

#endif
