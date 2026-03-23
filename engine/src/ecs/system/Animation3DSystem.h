//
// Created by adamd on 2026-03-16.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATION3DSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATION3DSYSTEM_H
#include <cmath>
#include <memory>
#include <vector>
#include <iostream>

#include "CalculateBoneTransform.h"
#include "Component.h"
#include "Entity.h"

class Animation3DSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float deltaTime)
    {
        for (auto& entity : entities)
        {
            if (!entity->hasComponent<Model>() ||
                !entity->hasComponent<Animation3D>() ||
                !entity->hasComponent<Animator>())
                continue;

            auto& model = entity->getComponent<Model>();
            auto& anim3D = entity->getComponent<Animation3D>();
            auto& animator = entity->getComponent<Animator>();

            animator.finalNodeTransforms.clear();

            if (!anim3D.clips || anim3D.clips->empty()) {
                std::cout << "Entity " << " has no animation clips\n";
                continue;
            }

            Animation3DClip& clip = (*anim3D.clips)[animator.currentClip];

            // Update animation time
            animator.currentTime += deltaTime * clip.ticksPerSecond;
            if (animator.currentTime >= clip.duration)
            {
                animator.currentTime -= clip.duration;
            }

            // std::cout << "Time: " << animator.currentTime
                // << " / " << clip.duration << "\n";

            // get root node index
            int rootIndex = model.rootBoneIndex;
            if (rootIndex < 0 || rootIndex >= (int)model.skeleton.size())
                continue;

            BoneNode& rootNode = model.skeleton[rootIndex];

            CalculateBoneTransform::Calculate(
                model,
                clip,
                animator
            );
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATION3DSYSTEM_H