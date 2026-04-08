/// @file TestAnimationSystem.cpp
/// @brief Tests for AnimationSystem state transitions, combo attacks, and blending.
///
/// Covers idle/run transitions, 3-hit combo advancement and wrapping,
/// combo window timing, hit-react and death states, blend completion,
/// and FinalBoneMatrices population. Uses a minimal one-bone skeleton.

#include "game/systems/AnimationSystem.h"
#include "game/systems/EntitySpawnSystem.h"
#include "game/components/Components.h"

#include "engine/ecs/Registry.h"
#include "engine/rendering/AnimationData.h"
#include "engine/resources/AudioEventQueue.h"

#include <glm/glm.hpp>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace
{
    /// Build a minimal skeleton with a single root bone for testing.
    std::shared_ptr<engine::Skeleton> MakeMinimalSkeleton()
    {
        auto skeleton = std::make_shared<engine::Skeleton>();

        engine::BoneNode root;
        root.Name = "Root";
        root.BoneId = 0;
        root.ParentIndex = -1;
        skeleton->Nodes.push_back(root);

        engine::BoneInfo info;
        info.Id = 0;
        info.Offset = glm::mat4(1.0f);
        skeleton->Bones.push_back(info);
        skeleton->BoneMap["Root"] = 0;
        skeleton->BoneCount = 1;
        skeleton->RootNodeIndex = 0;

        return skeleton;
    }

    /// Build an animation clip with one bone channel spanning the given duration.
    engine::AnimationClip MakeClip(const std::string& name, float duration)
    {
        engine::AnimationClip clip;
        clip.Name = name;
        clip.Duration = duration;
        clip.TicksPerSecond = 1.0f;

        // Add a bone channel so SamplePose has something to work with
        engine::AnimationBone bone;
        bone.Name = "Root";
        bone.Id = 0;
        bone.Positions = {{glm::vec3(0.0f), 0.0f}, {glm::vec3(0.0f), duration}};
        bone.Rotations = {{glm::quat(1, 0, 0, 0), 0.0f}, {glm::quat(1, 0, 0, 0), duration}};
        bone.Scales = {{glm::vec3(1.0f), 0.0f}, {glm::vec3(1.0f), duration}};
        clip.Bones["Root"] = bone;

        return clip;
    }

    /// Creates a Registry with a player entity that has all animation and combat
    /// components pre-configured with 7 clips (idle, run, 3 attacks, hit, death).
    struct TestSetup
    {
        engine::Registry Registry;
        engine::AudioEventQueue AudioQueue;
        engine::Entity PlayerEntity;

        TestSetup()
        {
            game::EntitySpawnSystem::Register(Registry);

            PlayerEntity = Registry.CreateEntity();
            Registry.AddComponent(PlayerEntity, game::Transform(0.0f, 0.0f, 0.0f));
            Registry.AddComponent(PlayerEntity, game::Player());

            auto clips = std::make_shared<std::vector<engine::AnimationClip>>();
            clips->push_back(MakeClip("Idle_A", 20.0f));           // 0
            clips->push_back(MakeClip("Running_A", 15.0f));        // 1
            clips->push_back(MakeClip("Attack_Slice", 10.0f));     // 2
            clips->push_back(MakeClip("Attack_Stab", 10.0f));      // 3
            clips->push_back(MakeClip("Attack_Jump", 10.0f));      // 4
            clips->push_back(MakeClip("Hit_A", 8.0f));             // 5
            clips->push_back(MakeClip("Death_A", 12.0f));          // 6

            game::AnimationState animState;
            animState.SkeletonPtr = MakeMinimalSkeleton();
            animState.Clips = clips;
            animState.IdleClipIndex = 0;
            animState.RunClipIndex = 1;
            animState.Attack1ClipIndex = 2;
            animState.Attack2ClipIndex = 3;
            animState.Attack3ClipIndex = 4;
            animState.HitClipIndex = 5;
            animState.DeathClipIndex = 6;
            animState.CurrentClip = 0;

            Registry.AddComponent(PlayerEntity, animState);
            Registry.AddComponent(PlayerEntity, game::CombatState());
        }
    };
}

int RunAnimationSystemTests()
{
    // Test: Initial state is Idle
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        assert(anim.CurrentState == game::AnimState::Idle);
        assert(anim.CurrentClip == 0); // IdleClipIndex
        assert(!anim.IsMoving);
        assert(!combat.IsAttacking);
    }

    // Test: Transition to Run state when IsMoving
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);

        anim.IsMoving = true;
        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(anim.CurrentState == game::AnimState::Run);
        assert(anim.IsBlending); // should blend to run clip
    }

    // Test: Return to Idle when not moving
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);

        // Move first
        anim.IsMoving = true;
        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);
        assert(anim.CurrentState == game::AnimState::Run);

        // Complete the blend
        anim.IsBlending = false;
        anim.CurrentClip = anim.RunClipIndex;

        // Stop moving
        anim.IsMoving = false;
        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);
        assert(anim.CurrentState == game::AnimState::Idle);
    }

    // Test: Attack starts combo at index 0
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IsAttacking = true;
        combat.ComboIndex = 0;
        anim.CurrentClip = anim.Attack1ClipIndex;
        anim.CurrentTime = 0.0f;

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(anim.CurrentState == game::AnimState::Attack1);
        assert(combat.IsAttacking);
    }

    // Test: Combo window opens at 50% of clip duration
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IsAttacking = true;
        combat.ComboIndex = 0;
        anim.CurrentClip = anim.Attack1ClipIndex;

        // Set time just past 50% of clip duration (10.0)
        // The system adds deltaTime * ticksPerSecond each frame
        anim.CurrentTime = 5.1f;
        combat.ComboWindowOpen = false;

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(combat.ComboWindowOpen);
        assert(combat.ComboTimer > 0.0f);
    }

    // Test: Queued attack advances combo
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IsAttacking = true;
        combat.ComboIndex = 0;
        anim.CurrentClip = anim.Attack1ClipIndex;
        combat.AttackQueued = true;

        // Set time past clip duration to trigger combo advance
        anim.CurrentTime = 10.1f;

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(combat.ComboIndex == 1);
        assert(!combat.AttackQueued);
        assert(anim.CurrentClip == anim.Attack2ClipIndex);
    }

    // Test: Combo wraps back to 0 after index 2
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IsAttacking = true;
        combat.ComboIndex = 2;
        anim.CurrentClip = anim.Attack3ClipIndex;
        combat.AttackQueued = true;
        anim.CurrentTime = 10.1f;

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(combat.ComboIndex == 0);
        assert(anim.CurrentClip == anim.Attack1ClipIndex);
    }

    // Test: Attack ends when no queued attack
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IsAttacking = true;
        combat.ComboIndex = 0;
        anim.CurrentClip = anim.Attack1ClipIndex;
        combat.AttackQueued = false;
        anim.CurrentTime = 10.1f;

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(!combat.IsAttacking);
        assert(combat.ComboIndex == 0);
    }

    // Test: Death state
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IsDead = true;
        anim.CurrentClip = anim.DeathClipIndex;
        anim.CurrentTime = 0.0f;

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(anim.CurrentState == game::AnimState::Death);
    }

    // Test: Death clamps time at clip duration
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IsDead = true;
        anim.CurrentClip = anim.DeathClipIndex;
        anim.CurrentTime = 13.0f; // past duration of 12.0

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        // Time should be clamped to clip duration
        assert(anim.CurrentTime <= 12.1f);
    }

    // Test: Hit react state
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IncomingHit = game::PendingHit{1, engine::Entity(0)};
        anim.CurrentClip = anim.HitClipIndex;
        anim.CurrentTime = 0.0f;

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(anim.CurrentState == game::AnimState::HitReact);
        assert(combat.IncomingHit.has_value());
    }

    // Test: Hit react ends when animation completes
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        auto& combat = setup.Registry.GetComponent<game::CombatState>(setup.PlayerEntity);

        combat.IncomingHit = game::PendingHit{1, engine::Entity(0)};
        anim.CurrentClip = anim.HitClipIndex;
        anim.CurrentTime = 8.1f; // past hit clip duration

        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        assert(!combat.IncomingHit.has_value());
    }

    // Test: FinalBoneMatrices populated after update
    {
        TestSetup setup;
        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);

        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);
        assert(anim.FinalBoneMatrices.size() == 100);
        assert(!anim.FinalNodeTransforms.empty());
    }

    // Test: Animation blending completes
    {
        TestSetup setup;
        auto& anim = setup.Registry.GetComponent<game::AnimationState>(setup.PlayerEntity);

        // Start a blend
        anim.IsMoving = true;
        game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);
        assert(anim.IsBlending);

        // Advance past blend duration (0.2s default)
        // Each update adds deltaTime to BlendTime
        for (int i = 0; i < 20; i++)
        {
            game::AnimationSystem::Update(setup.Registry, 0.016f, setup.AudioQueue);
        }

        assert(!anim.IsBlending);
        assert(anim.CurrentClip == anim.RunClipIndex);
    }

    std::cout << "Animation system tests passed.\n";
    return 0;
}
