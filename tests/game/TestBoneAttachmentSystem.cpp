/// @file TestBoneAttachmentSystem.cpp
/// @brief Tests for BoneAttachmentSystem, which parents child entities to skeleton bones.
///
/// Verifies that a child entity's ModelMatrix is computed as
/// parentWorldMatrix * boneTransform * offset. Also tests edge cases:
/// missing AnimationState on parent, nonexistent bone name, and parent rotation.

#include "game/systems/BoneAttachmentSystem.h"
#include "game/systems/EntitySpawnSystem.h"
#include "game/components/Components.h"

#include "engine/ecs/Registry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

namespace
{
    /// Returns true if two floats are within epsilon of each other.
    bool NearEqual(float a, float b, float epsilon = 0.01f)
    {
        return std::fabs(a - b) < epsilon;
    }
}

int RunBoneAttachmentTests()
{
    // Test: Child entity follows parent bone transform
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        // Create parent entity with animation state
        engine::Entity parent = registry.CreateEntity();
        registry.AddComponent(parent, game::Transform(5.0f, 0.0f, 3.0f));

        game::AnimationState animState;
        // Set up a bone transform for "handslot.r"
        glm::mat4 boneTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 0.0f));
        animState.FinalNodeTransforms["handslot.r"] = boneTransform;
        registry.AddComponent(parent, animState);

        // Create child entity attached to parent bone
        engine::Entity child = registry.CreateEntity();
        registry.AddComponent(child, game::Transform(0.0f, 0.0f, 0.0f));
        registry.AddComponent(child, game::BoneAttachment(parent, "handslot.r", glm::mat4(1.0f)));

        game::BoneAttachmentSystem::Update(registry);

        auto& childTransform = registry.GetComponent<game::Transform>(child);
        assert(childTransform.UseModelMatrix);

        // The child's ModelMatrix should be: parentWorldMatrix * boneTransform * offset
        // Parent is at (5, 0, 3) with no rotation/scale
        // Bone adds (1, 2, 0) relative to parent
        // Offset is identity
        // So final position should be (6, 2, 3)
        glm::vec4 pos = childTransform.ModelMatrix[3];
        assert(NearEqual(pos.x, 6.0f));
        assert(NearEqual(pos.y, 2.0f));
        assert(NearEqual(pos.z, 3.0f));
    }

    // Test: Child with offset
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity parent = registry.CreateEntity();
        registry.AddComponent(parent, game::Transform(0.0f, 0.0f, 0.0f));

        game::AnimationState animState;
        animState.FinalNodeTransforms["handslot.l"] = glm::mat4(1.0f);
        registry.AddComponent(parent, animState);

        glm::mat4 offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.15f));
        engine::Entity child = registry.CreateEntity();
        registry.AddComponent(child, game::Transform(0.0f, 0.0f, 0.0f));
        registry.AddComponent(child, game::BoneAttachment(parent, "handslot.l", offset));

        game::BoneAttachmentSystem::Update(registry);

        auto& childTransform = registry.GetComponent<game::Transform>(child);
        assert(childTransform.UseModelMatrix);

        glm::vec4 pos = childTransform.ModelMatrix[3];
        assert(NearEqual(pos.z, 0.15f));
    }

    // Test: Child skipped when parent is dead
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity parent = registry.CreateEntity();
        registry.AddComponent(parent, game::Transform(0.0f, 0.0f, 0.0f));
        // No AnimationState on parent

        engine::Entity child = registry.CreateEntity();
        registry.AddComponent(child, game::Transform(0.0f, 0.0f, 0.0f));
        registry.AddComponent(child, game::BoneAttachment(parent, "handslot.r", glm::mat4(1.0f)));

        game::BoneAttachmentSystem::Update(registry);

        auto& childTransform = registry.GetComponent<game::Transform>(child);
        assert(!childTransform.UseModelMatrix); // Should not have been set
    }

    // Test: Child skipped when bone not found
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity parent = registry.CreateEntity();
        registry.AddComponent(parent, game::Transform(0.0f, 0.0f, 0.0f));

        game::AnimationState animState;
        animState.FinalNodeTransforms["some_other_bone"] = glm::mat4(1.0f);
        registry.AddComponent(parent, animState);

        engine::Entity child = registry.CreateEntity();
        registry.AddComponent(child, game::Transform(0.0f, 0.0f, 0.0f));
        registry.AddComponent(child, game::BoneAttachment(parent, "nonexistent_bone", glm::mat4(1.0f)));

        game::BoneAttachmentSystem::Update(registry);

        auto& childTransform = registry.GetComponent<game::Transform>(child);
        assert(!childTransform.UseModelMatrix);
    }

    // Test: Parent rotation affects child
    {
        engine::Registry registry;
        game::EntitySpawnSystem::Register(registry);

        engine::Entity parent = registry.CreateEntity();
        game::Transform parentTransform(0.0f, 0.0f, 0.0f);
        parentTransform.RotationY = 3.14159265f / 2.0f; // 90 degrees
        registry.AddComponent(parent, parentTransform);

        game::AnimationState animState;
        // Bone at (1, 0, 0) relative to parent
        animState.FinalNodeTransforms["hand"] = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        registry.AddComponent(parent, animState);

        engine::Entity child = registry.CreateEntity();
        registry.AddComponent(child, game::Transform(0.0f, 0.0f, 0.0f));
        registry.AddComponent(child, game::BoneAttachment(parent, "hand", glm::mat4(1.0f)));

        game::BoneAttachmentSystem::Update(registry);

        auto& childTransform = registry.GetComponent<game::Transform>(child);
        assert(childTransform.UseModelMatrix);

        // Parent rotated 90 degrees around Y, bone at (1,0,0)
        // After rotation: bone should be approximately at (0, 0, -1) in world space
        glm::vec4 pos = childTransform.ModelMatrix[3];
        assert(NearEqual(pos.x, 0.0f, 0.1f));
        assert(NearEqual(pos.z, -1.0f, 0.1f));
    }

    std::cout << "Bone attachment system tests passed.\n";
    return 0;
}
