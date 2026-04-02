// Created by Elijah Fabon

#include "game/systems/AnimationSystem.h"

#include "game/components/Components.h"

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

        int GetComboClipIndex(const AnimationState& anim, int comboIndex)
        {
            switch (comboIndex)
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

            // Read combat state if present (otherwise defaults to no combat)
            bool isAttacking = false;
            bool attackQueued = false;
            bool comboWindowOpen = false;
            int comboIndex = 0;
            float comboWindow = 0.5f;
            float comboTimer = 0.0f;
            bool isHit = false;
            bool isDead = false;

            bool hasCombat = registry.HasComponent<CombatState>(entity);
            if (hasCombat)
            {
                auto& combat = registry.GetComponent<CombatState>(entity);
                isAttacking = combat.IsAttacking;
                attackQueued = combat.AttackQueued;
                comboWindowOpen = combat.ComboWindowOpen;
                comboIndex = combat.ComboIndex;
                comboWindow = combat.ComboWindow;
                comboTimer = combat.ComboTimer;
                isHit = combat.IncomingHit.has_value();
                isDead = combat.IsDead;
            }

            engine::AnimationClip& clip = (*anim.Clips)[anim.CurrentClip];
            anim.CurrentTime += deltaTime * clip.TicksPerSecond;

            if (isDead)
            {
                if (anim.CurrentTime >= clip.Duration)
                    anim.CurrentTime = clip.Duration;
            }

            if (isHit)
            {
                if (anim.CurrentTime >= clip.Duration && hasCombat)
                    registry.GetComponent<CombatState>(entity).IncomingHit.reset();
            }

            if (isAttacking && hasCombat)
            {
                auto& combat = registry.GetComponent<CombatState>(entity);

                if (combat.ComboTimer > 0.0f)
                    combat.ComboTimer -= deltaTime;

                float comboStart = clip.Duration * 0.5f;

                if (anim.CurrentTime >= comboStart &&
                    anim.CurrentTime < clip.Duration * 0.9f &&
                    !combat.ComboWindowOpen)
                {
                    combat.ComboWindowOpen = true;
                    combat.ComboTimer = combat.ComboWindow;
                }

                if (combat.ComboWindowOpen &&
                    !(anim.CurrentTime >= comboStart && anim.CurrentTime < clip.Duration * 0.9f))
                {
                    combat.ComboWindowOpen = false;
                }

                if (anim.CurrentTime >= clip.Duration)
                {
                    if (combat.AttackQueued)
                    {
                        combat.ComboIndex++;
                        combat.AttackQueued = false;
                        anim.CurrentTime = 0.0f;

                        if (combat.ComboIndex > 2)
                            combat.ComboIndex = 0;

                        int nextAttackClip = GetComboClipIndex(anim, combat.ComboIndex);
                        if (nextAttackClip >= 0)
                            anim.CurrentClip = nextAttackClip;
                    }
                    else
                    {
                        combat.IsAttacking = false;
                        combat.ComboIndex = 0;
                        combat.ComboTimer = 0.0f;
                    }
                }
            }

            // State machine
            AnimState targetState = AnimState::Idle;

            if (isDead)
                targetState = AnimState::Death;
            else if (isHit)
                targetState = AnimState::HitReact;
            else if (isAttacking)
            {
                switch (comboIndex)
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

            if (anim.CurrentTime >= clip.Duration && !isAttacking && !isDead)
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
