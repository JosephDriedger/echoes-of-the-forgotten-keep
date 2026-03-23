#include "game/systems/CollisionSystem.h"

namespace game
{
    void CollisionSystem::Update(engine::Registry& registry) const
    {
        const std::vector<engine::Entity>& entities = registry.GetActiveEntities();

        for (std::size_t i = 0; i < entities.size(); ++i)
        {
            const engine::Entity entityA = entities[i];

            if (!registry.HasComponent<Transform>(entityA) || !registry.HasComponent<Collider>(entityA))
            {
                continue;
            }

            Transform& transformA = registry.GetComponent<Transform>(entityA);
            const Collider& colliderA = registry.GetComponent<Collider>(entityA);

            for (std::size_t j = i + 1; j < entities.size(); ++j)
            {
                const engine::Entity entityB = entities[j];

                if (!registry.HasComponent<Transform>(entityB) || !registry.HasComponent<Collider>(entityB))
                {
                    continue;
                }

                Transform& transformB = registry.GetComponent<Transform>(entityB);
                const Collider& colliderB = registry.GetComponent<Collider>(entityB);

                if (!IsColliding(transformA, colliderA, transformB, colliderB))
                {
                    continue;
                }

                if (!colliderA.IsTrigger && !colliderB.IsTrigger)
                {
                    ResolveCollision(transformA, colliderA, transformB, colliderB);
                }
            }
        }
    }

    bool CollisionSystem::IsColliding(const Transform& transformA,
                                      const Collider& colliderA,
                                      const Transform& transformB,
                                      const Collider& colliderB)
    {
        return transformA.X < transformB.X + colliderB.Width &&
               transformA.X + colliderA.Width > transformB.X &&
               transformA.Y < transformB.Y + colliderB.Height &&
               transformA.Y + colliderA.Height > transformB.Y &&
               transformA.Z < transformB.Z + colliderB.Depth &&
               transformA.Z + colliderA.Depth > transformB.Z;
    }

    void CollisionSystem::ResolveCollision(Transform& transformA,
                                           const Collider& colliderA,
                                           Transform& transformB,
                                           const Collider& colliderB)
    {
        const float overlapLeft = (transformA.X + colliderA.Width) - transformB.X;
        const float overlapRight = (transformB.X + colliderB.Width) - transformA.X;
        const float overlapDown = (transformA.Y + colliderA.Height) - transformB.Y;
        const float overlapUp = (transformB.Y + colliderB.Height) - transformA.Y;

        float minimumOverlap = overlapLeft;

        enum class ResolutionAxis
        {
            Left,
            Right,
            Down,
            Up
        } resolutionAxis = ResolutionAxis::Left;

        if (overlapRight < minimumOverlap)
        {
            minimumOverlap = overlapRight;
            resolutionAxis = ResolutionAxis::Right;
        }

        if (overlapDown < minimumOverlap)
        {
            minimumOverlap = overlapDown;
            resolutionAxis = ResolutionAxis::Down;
        }

        if (overlapUp < minimumOverlap)
        {
            minimumOverlap = overlapUp;
            resolutionAxis = ResolutionAxis::Up;
        }

        const float halfOverlap = minimumOverlap * 0.5f;

        switch (resolutionAxis)
        {
        case ResolutionAxis::Left:
            transformA.X -= halfOverlap;
            transformB.X += halfOverlap;
            break;

        case ResolutionAxis::Right:
            transformA.X += halfOverlap;
            transformB.X -= halfOverlap;
            break;

        case ResolutionAxis::Down:
            transformA.Y -= halfOverlap;
            transformB.Y += halfOverlap;
            break;

        case ResolutionAxis::Up:
            transformA.Y += halfOverlap;
            transformB.Y -= halfOverlap;
            break;
        }
    }
}