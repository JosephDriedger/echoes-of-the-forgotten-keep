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
#include "AnimationKey.h"
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

            Animation3DClip& clip = (*anim3D.clips)[animator.currentClip];
            animator.currentTime += deltaTime * clip.ticksPerSecond;

            if (animator.isAttacking) {

                if (animator.comboTimer > 0.0f) {
                    animator.comboTimer -= deltaTime;
                }

                float clipDuration = clip.duration;

                float comboStart = clipDuration * 0.5f;

                if (animator.currentTime >= comboStart && animator.currentTime < clipDuration * 0.9f && !animator.comboWindowOpen) {
                    animator.comboWindowOpen = true;
                    animator.comboTimer = animator.comboWindow;
                }
                if (animator.comboWindowOpen && !(animator.currentTime >= comboStart && animator.currentTime < clipDuration * 0.9f)) {
                    animator.comboWindowOpen = false;
                }

                // 🎯 End of attack
                if (animator.currentTime >= clipDuration) {

                    if (animator.attackQueued) {
                        // 🔥 chain combo
                        animator.comboIndex++;

                        animator.attackQueued = false;
                        animator.currentTime = 0.0f;

                        if (animator.comboIndex > 2)  // max combo
                            animator.comboIndex = 0;

                    } else {
                        // end attack
                        animator.isAttacking = false;
                        animator.comboIndex = 0;
                        animator.comboTimer = 0.0f;
                    }
                }
            }

            animator.finalNodeTransforms.clear();

            if (!anim3D.clips || anim3D.clips->empty()) {
                std::cout << "Entity " << " has no animation clips\n";
                continue;
            }

            // Update animation time

            if (animator.currentTime >= clip.duration)
            {
                animator.currentTime -= clip.duration;
            }

            if (animator.isBlending) {
                animator.nextTime += deltaTime;
                animator.blendTime += deltaTime;


                if (animator.blendTime / animator.blendDuration >= 1.0f) {
                    animator.currentClip = animator.nextClip;
                    animator.currentTime = animator.nextTime;

                    animator.isBlending = false;
                    animator.nextClip = -1;
                }
            }

            // get root node index
            int rootIndex = model.rootBoneIndex;
            if (rootIndex < 0 || rootIndex >= (int)model.skeleton.size())
                continue;

            BoneNode& rootNode = model.skeleton[rootIndex];

            UpdateAnimation(
                anim3D,
                model,
                animator
            );
        }
    }

    void UpdateAnimation(Animation3D& animation, Model& model, Animator& animator)
    {
        const Animation3DClip& currentClip = animation.clips->at(animator.currentClip);

        Pose currentPose = CalculateBoneTransform::SamplePose(model, currentClip, animator.currentTime);

        if (animator.isBlending && animator.nextClip != -1)
        {
            const Animation3DClip& nextClip = animation.clips->at(animator.nextClip);

            Pose nextPose = CalculateBoneTransform::SamplePose(model, nextClip, animator.nextTime);

            float t = animator.blendTime / animator.blendDuration;
            t = glm::clamp(t, 0.0f, 1.0f);

            Pose blended = CalculateBoneTransform::BlendPoses(currentPose, nextPose, t);

            CalculateBoneTransform::ApplyPose(model, blended, animator);
        }
        else
        {
            CalculateBoneTransform::ApplyPose(model, currentPose, animator);
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATION3DSYSTEM_H