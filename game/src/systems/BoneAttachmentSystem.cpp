// Created by Elijah Fabon

#include "game/systems/BoneAttachmentSystem.h"

#include "game/components/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace game
{
    void BoneAttachmentSystem::Update(engine::Registry& registry)
    {
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<BoneAttachment>(entity) ||
                !registry.HasComponent<Transform>(entity))
            {
                continue;
            }

            auto& attach = registry.GetComponent<BoneAttachment>(entity);
            engine::Entity parent = attach.ParentEntity;

            if (!registry.IsAlive(parent) ||
                !registry.HasComponent<AnimationState>(parent) ||
                !registry.HasComponent<Transform>(parent))
            {
                continue;
            }

            const auto& parentAnim = registry.GetComponent<AnimationState>(parent);
            const auto& parentTransform = registry.GetComponent<Transform>(parent);

            auto it = parentAnim.FinalNodeTransforms.find(attach.BoneName);
            if (it == parentAnim.FinalNodeTransforms.end())
            {
                continue;
            }

            glm::mat4 boneTransform = it->second;

            // Build parent world matrix
            glm::mat4 parentMatrix = glm::mat4(1.0f);
            parentMatrix = glm::translate(parentMatrix,
                glm::vec3(parentTransform.X, parentTransform.Y, parentTransform.Z));
            parentMatrix = glm::rotate(parentMatrix, parentTransform.RotationY,
                glm::vec3(0.0f, 1.0f, 0.0f));
            parentMatrix = glm::scale(parentMatrix,
                glm::vec3(parentTransform.ScaleX, parentTransform.ScaleY, parentTransform.ScaleZ));

            glm::mat4 finalTransform = parentMatrix * boneTransform * attach.Offset;

            auto& transform = registry.GetComponent<Transform>(entity);
            transform.ModelMatrix = finalTransform;
            transform.UseModelMatrix = true;
        }
    }
}
