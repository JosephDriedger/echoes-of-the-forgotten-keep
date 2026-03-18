//
// Created by adamd on 2026-03-17.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACHMENTSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACHMENTSYSTEM_H
#include <iostream>
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class AttachmentSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities)
    {
        for (auto& entity : entities)
        {
            if (!entity->hasComponent<BoneAttachment>() ||
                !entity->hasComponent<Transform3D>())
                continue;

            auto& attach = entity->getComponent<BoneAttachment>();

            if (!attach.parent ||
                !attach.parent->hasComponent<Animator>() || !attach.parent->hasComponent<Transform3D>())
                continue;

            auto& parentAnimator = attach.parent->getComponent<Animator>();
            auto& parentTransform = attach.parent->getComponent<Transform3D>();

            auto it = parentAnimator.finalNodeTransforms.find(attach.boneName);

            if (it == parentAnimator.finalNodeTransforms.end())
            {
                std::cout << "Bone not found: "
                          << attach.boneName << "\n";
                continue;
            }

            glm::mat4 boneTransform = it->second;

            // 🔥 Build player world matrix
            glm::mat4 playerMatrix = glm::mat4(1.0f);
            playerMatrix = glm::translate(playerMatrix, parentTransform.position);
            playerMatrix = glm::rotate(playerMatrix, glm::radians(parentTransform.rotation.x), glm::vec3(1,0,0));
            playerMatrix = glm::rotate(playerMatrix, glm::radians(parentTransform.rotation.y), glm::vec3(0,1,0));
            playerMatrix = glm::rotate(playerMatrix, glm::radians(parentTransform.rotation.z), glm::vec3(0,0,1));
            playerMatrix = glm::scale(playerMatrix, parentTransform.scale);

            // apply offset
            glm::mat4 finalTransform = playerMatrix * boneTransform * attach.offset;

            auto& transform = entity->getComponent<Transform3D>();

            // 🔥 Apply transform
            transform.modelMatrix = finalTransform;
            transform.useMatrix = true;
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACHMENTSYSTEM_H