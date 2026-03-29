#include "game/systems/AnimationSystem.h"

#include "game/components/AnimationState.h"

#include "engine/rendering/Animator.h"

#include <glm/glm.hpp>

#include <iostream>

namespace game
{
    namespace
    {
        void BlendToClip(AnimationState& anim, int clipIndex)
        {
            if (clipIndex < 0 || !anim.Clips || clipIndex >= static_cast<int>(anim.Clips->size()))
                return;

            if (anim.CurrentClip == clipIndex && !anim.IsBlending)
                return;

            anim.NextClip = clipIndex;
            anim.NextTime = 0.0f;
            anim.BlendTime = 0.0f;
            anim.IsBlending = true;
        }

        int GetComboClipIndex(const AnimationState& anim)
        {
            switch (anim.ComboIndex)
            {
                case 0: return anim.Attack1ClipIndex;
                case 1: return anim.Attack2ClipIndex;
                case 2: return anim.Attack3ClipIndex;
                default: return anim.Attack1ClipIndex;
            }
        }
    }

    void AnimationSystem::Update(engine::Registry& registry, float deltaTime)
    {
        for (const engine::Entity entity : registry.GetActiveEntities())
        {
            if (!registry.HasComponent<AnimationState>(entity))
                continue;

            auto& anim = registry.GetComponent<AnimationState>(entity);

            if (!anim.Clips || anim.Clips->empty() || !anim.SkeletonPtr)
                continue;

            engine::AnimationClip& clip = (*anim.Clips)[anim.CurrentClip];
            anim.CurrentTime += deltaTime * clip.TicksPerSecond;

            if (anim.IsDead)
            {
                if (anim.CurrentTime >= clip.Duration)
                    anim.CurrentTime = clip.Duration;
            }

            if (anim.IsHit)
            {
                if (anim.CurrentTime >= clip.Duration)
                    anim.IsHit = false;
            }

            if (anim.IsAttacking)
            {
                if (anim.ComboTimer > 0.0f)
                    anim.ComboTimer -= deltaTime;

                float comboStart = clip.Duration * 0.5f;

                if (anim.CurrentTime >= comboStart &&
                    anim.CurrentTime < clip.Duration * 0.9f &&
                    !anim.ComboWindowOpen)
                {
                    anim.ComboWindowOpen = true;
                    anim.ComboTimer = anim.ComboWindow;
                }

                if (anim.ComboWindowOpen &&
                    !(anim.CurrentTime >= comboStart && anim.CurrentTime < clip.Duration * 0.9f))
                {
                    anim.ComboWindowOpen = false;
                }

                if (anim.CurrentTime >= clip.Duration)
                {
                    if (anim.AttackQueued)
                    {
                        anim.ComboIndex++;
                        anim.AttackQueued = false;
                        anim.CurrentTime = 0.0f;

                        if (anim.ComboIndex > 2)
                            anim.ComboIndex = 0;

                        int nextAttackClip = GetComboClipIndex(anim);
                        if (nextAttackClip >= 0)
                            anim.CurrentClip = nextAttackClip;
                    }
                    else
                    {
                        anim.IsAttacking = false;
                        anim.ComboIndex = 0;
                        anim.ComboTimer = 0.0f;
                    }
                }
            }

            // State machine
            AnimState targetState = AnimState::Idle;

            if (anim.IsDead)
                targetState = AnimState::Death;
            else if (anim.IsHit)
                targetState = AnimState::HitReact;
            else if (anim.IsAttacking)
            {
                switch (anim.ComboIndex)
                {
                    case 0: targetState = AnimState::Attack1; break;
                    case 1: targetState = AnimState::Attack2; break;
                    case 2: targetState = AnimState::Attack3; break;
                    default: targetState = AnimState::Attack1; break;
                }
            }
            else if (anim.IsMoving)
                targetState = AnimState::Run;

            if (targetState != anim.CurrentState)
            {
                anim.PreviousState = anim.CurrentState;
                anim.CurrentState = targetState;

                int clipIndex = -1;
                switch (targetState)
                {
                    case AnimState::Idle: clipIndex = anim.IdleClipIndex; break;
                    case AnimState::Run: clipIndex = anim.RunClipIndex; break;
                    case AnimState::Attack1: clipIndex = anim.Attack1ClipIndex; break;
                    case AnimState::Attack2: clipIndex = anim.Attack2ClipIndex; break;
                    case AnimState::Attack3: clipIndex = anim.Attack3ClipIndex; break;
                    case AnimState::HitReact: clipIndex = anim.HitClipIndex; break;
                    case AnimState::Death: clipIndex = anim.DeathClipIndex; break;
                }

                if (clipIndex >= 0)
                {
                    BlendToClip(anim, clipIndex);
                }
            }

            anim.FinalNodeTransforms.clear();

            if (anim.CurrentTime >= clip.Duration && !anim.IsAttacking && !anim.IsDead)
            {
                anim.CurrentTime -= clip.Duration;
            }

            if (anim.IsBlending)
            {
                anim.NextTime += deltaTime * clip.TicksPerSecond;
                anim.BlendTime += deltaTime;

                if (anim.BlendTime / anim.BlendDuration >= 1.0f)
                {
                    anim.CurrentClip = anim.NextClip;
                    anim.CurrentTime = anim.NextTime;
                    anim.IsBlending = false;
                    anim.NextClip = -1;
                }
            }

            // Compute bone transforms
            const engine::AnimationClip& currentClip = (*anim.Clips)[anim.CurrentClip];
            engine::Pose currentPose = engine::Animator::SamplePose(*anim.SkeletonPtr, currentClip, anim.CurrentTime);

            if (anim.IsBlending && anim.NextClip >= 0)
            {
                const engine::AnimationClip& nextClip = (*anim.Clips)[anim.NextClip];
                engine::Pose nextPose = engine::Animator::SamplePose(*anim.SkeletonPtr, nextClip, anim.NextTime);

                float t = glm::clamp(anim.BlendTime / anim.BlendDuration, 0.0f, 1.0f);
                engine::Pose blended = engine::Animator::BlendPoses(currentPose, nextPose, t);

                engine::Animator::ApplyPose(*anim.SkeletonPtr, blended,
                    anim.FinalBoneMatrices, anim.FinalNodeTransforms);
            }
            else
            {
                engine::Animator::ApplyPose(*anim.SkeletonPtr, currentPose,
                    anim.FinalBoneMatrices, anim.FinalNodeTransforms);
            }
        }
    }
}
