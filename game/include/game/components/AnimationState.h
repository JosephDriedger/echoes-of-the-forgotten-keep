#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSTATE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSTATE_H

#include "engine/rendering/AnimationData.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace game
{
    enum class AnimState
    {
        Idle,
        Run,
        Attack1,
        Attack2,
        Attack3,
        HitReact,
        Death
    };

    struct AnimationState
    {
        std::shared_ptr<engine::Skeleton> SkeletonPtr;
        std::shared_ptr<std::vector<engine::AnimationClip>> Clips;

        int CurrentClip = 0;
        int NextClip = -1;

        float CurrentTime = 0.0f;
        float NextTime = 0.0f;

        float BlendTime = 0.0f;
        float BlendDuration = 0.2f;
        bool IsBlending = false;

        bool IsAttacking = false;
        bool AttackQueued = false;
        bool ComboWindowOpen = false;
        int ComboIndex = 0;
        float ComboWindow = 0.5f;
        float ComboTimer = 0.0f;

        bool IsHit = false;
        bool IsDead = false;
        int Health = 3;
        float HitTimer = 0.0f;

        bool IsMoving = false;

        AnimState CurrentState = AnimState::Idle;
        AnimState PreviousState = AnimState::Idle;

        std::vector<glm::mat4> FinalBoneMatrices;
        std::unordered_map<std::string, glm::mat4> FinalNodeTransforms;

        int IdleClipIndex = -1;
        int RunClipIndex = -1;
        int Attack1ClipIndex = -1;
        int Attack2ClipIndex = -1;
        int Attack3ClipIndex = -1;
        int HitClipIndex = -1;
        int DeathClipIndex = -1;

        AnimationState()
            : FinalBoneMatrices(100, glm::mat4(1.0f))
        {
        }
    };
}

#endif
