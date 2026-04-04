// Created by Elijah Fabon

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_BONEATTACHMENTSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_BONEATTACHMENTSYSTEM_H

#include "engine/ecs/Registry.h"

namespace game
{
    class BoneAttachmentSystem
    {
    public:
        static void Update(engine::Registry& registry);
    };
}

#endif
