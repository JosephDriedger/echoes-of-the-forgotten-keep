// Created by Adam Van Woerden
/// @file Components.h
/// @brief All ECS component structs used by the game.
///
/// Components are plain data structs attached to entities via the Registry.
/// They hold no behavior -- systems operate on them each frame.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_H

#include "engine/ecs/Entity.h"
#include "engine/rendering/AnimationData.h"
#include "engine/rendering/Mesh.h"
#include "engine/rendering/Texture.h"
#include "ComponentEnum.h"
#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace game
{
    // -----------------------------------------------------------------------
    // Transform -- position, rotation, and scale in 3D world space.
    // -----------------------------------------------------------------------
    struct Transform
    {
        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;

        float RotationY = 0.0f;

        float ScaleX = 1.0f;
        float ScaleY = 1.0f;
        float ScaleZ = 1.0f;

        glm::mat4 ModelMatrix = glm::mat4(1.0f);
        bool UseModelMatrix = false;

        Transform() = default;

        Transform(const float x, const float y, const float z = 0.0f)
            : X(x), Y(y), Z(z)
        {
        }
    };

    // -----------------------------------------------------------------------
    // Render -- references to GPU mesh and texture for drawing.
    // -----------------------------------------------------------------------
    struct Render
    {
        std::shared_ptr<engine::Mesh> MeshPtr;
        std::shared_ptr<engine::Texture> TexturePtr;

        Render() = default;

        Render(std::shared_ptr<engine::Mesh> mesh, std::shared_ptr<engine::Texture> texture)
            : MeshPtr(std::move(mesh)), TexturePtr(std::move(texture))
        {
        }
    };

    // -----------------------------------------------------------------------
    // Player -- empty marker that identifies the player entity.
    // -----------------------------------------------------------------------
    struct Player {};

    // -----------------------------------------------------------------------
    // Health -- current and maximum hit points.
    // -----------------------------------------------------------------------
    struct Health
    {
        int Current;
        int Maximum;

        Health()
            : Current(100),
              Maximum(100)
        {
        }

        Health(const int current, const int maximum)
            : Current(current),
              Maximum(maximum)
        {
        }
    };

    // -----------------------------------------------------------------------
    // Collider -- axis-aligned bounding box for collision detection.
    // -----------------------------------------------------------------------
    struct Collider
    {
        float Width;
        float Height;
        float Depth;

        // Offset from entity position to collider center (world space)
        float OffsetX = 0.0f;
        float OffsetY = 0.0f;
        float OffsetZ = 0.0f;

        bool IsTrigger;
        bool IsStatic;

        Collider()
            : Width(1.0f), Height(1.0f), Depth(1.0f), IsTrigger(false), IsStatic(false)
        {
        }

        Collider(const float width, const float height, const float depth, const bool isTrigger = false)
            : Width(width), Height(height), Depth(depth), IsTrigger(isTrigger), IsStatic(false)
        {
        }

        Collider(const float width, const float height)
            : Width(width), Height(height), Depth(1.0f), IsTrigger(false), IsStatic(false)
        {
        }
    };

    struct EnemyAI
    {
        float MoveSpeed = 1.0f;

        AIState State = AIState::Idle;
        float StateTimer = 0.0f;

        float VisionRange = 10.0f;
        float AttackRange = 2.0f;
        float AttackCooldown = 2.0f;
        float AttackTimer = 0.0f;

        float MaxChaseTime = 5.0f;
        float LoseTargetCooldown = 2.0f;
        float LoseTargetTimer = 0.0f;

        // Patrol waypoints
        bool HasPatrol = false;
        float PatrolAX = 0.0f, PatrolAZ = 0.0f;
        float PatrolBX = 0.0f, PatrolBZ = 0.0f;
        bool MovingToB = true;
        float IdleBeforePatrol = 2.0f;

        // Knockback
        bool IsKnockedBack = false;
        float KnockbackTimer = 0.0f;
        float KnockbackDuration = 0.7f;
        float KnockbackVX = 0.0f;
        float KnockbackVZ = 0.0f;
        float KnockbackSpeed = 4.0f;

        explicit EnemyAI(const float moveSpeed = 1.0f)
            : MoveSpeed(moveSpeed)
        {
        }
    };

    // -----------------------------------------------------------------------
    // Door -- swing state and puzzle trigger linkage.
    // -----------------------------------------------------------------------
    struct Door
    {
        float BaseRotationY = 0.0f;   // original rotation when spawned
        float CurrentAngle = 0.0f;    // current swing angle in degrees (0 = closed)
        float SwingSpeed = 180.0f;    // degrees per second
        int SwingDirection = 0;       // +1 or -1, determined by player approach side

        // Trigger/puzzle-based door fields
        std::string TriggerId;        // links to Switch::Id; empty = proximity door
        bool Open = false;
        float TargetAngle = 0.0f;
        float OpenAngle = 90.0f;
        float CloseAngle = 0.0f;

        // Physical collider entity that moves with the door
        engine::Entity ColliderEntity{0};
        float PanelLength = 1.6f;     // width of the door panel
        float PanelThickness = 0.2f;  // thickness of the door panel

        Door() = default;
    };

    // -----------------------------------------------------------------------
    // Switch -- floor trigger for puzzles and doors.
    // -----------------------------------------------------------------------
    struct Switch
    {
        std::string Id;
        bool Pressed = false;

        Switch() = default;
        explicit Switch(std::string id) : Id(std::move(id)) {}
    };

    // -----------------------------------------------------------------------
    // BoneAttachment -- attaches a child entity to a parent's skeleton bone.
    // -----------------------------------------------------------------------
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

    // -----------------------------------------------------------------------
    // Lifetime -- auto-destroys the entity after Duration seconds.
    // -----------------------------------------------------------------------
    struct Lifetime
    {
        float Duration = 5.0f;

        Lifetime() = default;
        explicit Lifetime(float duration) : Duration(duration) {}
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

        bool IsMoving = false;

        float AnimationSpeed = 1.0f;

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

    struct AttackStateComponent
    {
        engine::Entity ActiveHitbox{0};
        bool HasActiveHitbox = false;
        std::unordered_set<uint32_t> HitThisSwing;
        int LastComboIndex = -1;
    };

    struct CameraFollowState
    {
        float Zoom = 20.0f;
    };

    struct CombatState
    {
        int AttackDamage = 1;

        bool IsAttacking = false;
        bool AttackQueued = false;
        bool ComboWindowOpen = false;
        int ComboIndex = 0;
        float ComboWindow = 0.5f;
        float ComboTimer = 0.0f;
        float FinalHitKnockbackMultiplier = 2.5f;

        bool IsLunging = false;
        float LungeDuration = 0.7f;
        float LungeTimer = 0.0f;
        float LungeSpeed = 7.0f;

        std::optional<PendingHit> IncomingHit;
        bool IsDead = false;
        float HitTimer = 0.0f;

        CombatState() = default;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_H
