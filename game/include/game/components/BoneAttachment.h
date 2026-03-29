#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_BONEATTACHMENT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_BONEATTACHMENT_H

#include "engine/ecs/Entity.h"

#include <glm/glm.hpp>

#include <string>

namespace game
{
    struct BoneAttachment
    {
        engine::Entity ParentEntity;
        std::string BoneName;
        glm::mat4 Offset = glm::mat4(1.0f);

        BoneAttachment()
            : ParentEntity(0)
        {
        }

        BoneAttachment(engine::Entity parent, const std::string& boneName,
                        const glm::mat4& offset = glm::mat4(1.0f))
            : ParentEntity(parent), BoneName(boneName), Offset(offset)
        {
        }
    };
}

#endif
