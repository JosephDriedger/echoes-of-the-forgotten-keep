// Created by Elijah Fabon

#include "game/systems/CollisionSystem.h"

#include <cmath>

namespace game
{
    void CollisionSystem::Update(engine::Registry& registry)
    {
        auto players = registry.View<Player, Transform, Collider>();
        auto enemies = registry.View<EnemyAI, Transform, Collider>();
        auto world   = registry.View<Transform, Collider>();

        for (auto player : players)
        {
            auto& pTransform = registry.GetComponent<Transform>(player);
            auto& pCollider  = registry.GetComponent<Collider>(player);

            for (auto other : world)
            {
                if (other == player)
                    continue;

                if (!registry.HasComponent<Transform>(other) ||
                    !registry.HasComponent<Collider>(other))
                    continue;

                auto& oTransform = registry.GetComponent<Transform>(other);
                auto& oCollider  = registry.GetComponent<Collider>(other);

                if (!IsColliding(pTransform, pCollider, oTransform, oCollider))
                    continue;

                if (!pCollider.IsTrigger && !oCollider.IsTrigger)
                {
                    ResolveCollision(pTransform, pCollider, oTransform, oCollider);
                }
            }
        }

        for (auto enemy : enemies)
        {
            auto& eTransform = registry.GetComponent<Transform>(enemy);
            auto& eCollider  = registry.GetComponent<Collider>(enemy);

            for (auto other : world)
            {
                if (other == enemy)
                    continue;

                auto& oTransform = registry.GetComponent<Transform>(other);
                auto& oCollider  = registry.GetComponent<Collider>(other);

                if (!IsColliding(eTransform, eCollider, oTransform, oCollider))
                    continue;

                if (!eCollider.IsTrigger && !oCollider.IsTrigger)
                {
                    ResolveCollision(eTransform, eCollider, oTransform, oCollider);
                }
            }
        }
        // const std::vector<engine::Entity>& entities = registry.GetActiveEntities();
        //
        // for (std::size_t i = 0; i < entities.size(); ++i)
        // {
        //     const engine::Entity entityA = entities[i];
        //
        //     if (!registry.HasComponent<Transform>(entityA) || !registry.HasComponent<Collider>(entityA))
        //         continue;
        //
        //     Transform& transformA = registry.GetComponent<Transform>(entityA);
        //     const Collider& colliderA = registry.GetComponent<Collider>(entityA);
        //
        //     for (std::size_t j = i + 1; j < entities.size(); ++j)
        //     {
        //         const engine::Entity entityB = entities[j];
        //
        //         if (!registry.HasComponent<Transform>(entityB) || !registry.HasComponent<Collider>(entityB))
        //             continue;
        //
        //         // Skip if both are static
        //         if (colliderA.IsStatic && registry.GetComponent<Collider>(entityB).IsStatic)
        //             continue;
        //
        //         Transform& transformB = registry.GetComponent<Transform>(entityB);
        //         const Collider& colliderB = registry.GetComponent<Collider>(entityB);
        //
        //         if (!IsColliding(transformA, colliderA, transformB, colliderB))
        //             continue;
        //
        //         if (!colliderA.IsTrigger && !colliderB.IsTrigger)
        //         {
        //             ResolveCollision(transformA, colliderA, transformB, colliderB);
        //         }
        //     }
        // }
    }

    bool CollisionSystem::IsColliding(const Transform& transformA,
                                      const Collider& colliderA,
                                      const Transform& transformB,
                                      const Collider& colliderB)
    {
        // Center-based AABB with offsets
        float aCenterX = transformA.X + colliderA.OffsetX;
        float aCenterY = transformA.Y + colliderA.OffsetY;
        float aCenterZ = transformA.Z + colliderA.OffsetZ;

        float bCenterX = transformB.X + colliderB.OffsetX;
        float bCenterY = transformB.Y + colliderB.OffsetY;
        float bCenterZ = transformB.Z + colliderB.OffsetZ;

        float aMinX = aCenterX - colliderA.Width * 0.5f;
        float aMaxX = aCenterX + colliderA.Width * 0.5f;
        float aMinY = aCenterY;
        float aMaxY = aCenterY + colliderA.Height;
        float aMinZ = aCenterZ - colliderA.Depth * 0.5f;
        float aMaxZ = aCenterZ + colliderA.Depth * 0.5f;

        float bMinX = bCenterX - colliderB.Width * 0.5f;
        float bMaxX = bCenterX + colliderB.Width * 0.5f;
        float bMinY = bCenterY;
        float bMaxY = bCenterY + colliderB.Height;
        float bMinZ = bCenterZ - colliderB.Depth * 0.5f;
        float bMaxZ = bCenterZ + colliderB.Depth * 0.5f;

        return aMinX < bMaxX && aMaxX > bMinX &&
               aMinY < bMaxY && aMaxY > bMinY &&
               aMinZ < bMaxZ && aMaxZ > bMinZ;
    }

    void CollisionSystem::ResolveCollision(Transform& transformA,
                                           const Collider& colliderA,
                                           Transform& transformB,
                                           const Collider& colliderB)
    {
        float aCenterX = transformA.X + colliderA.OffsetX;
        float aCenterZ = transformA.Z + colliderA.OffsetZ;
        float bCenterX = transformB.X + colliderB.OffsetX;
        float bCenterZ = transformB.Z + colliderB.OffsetZ;

        float overlapX = (colliderA.Width + colliderB.Width) * 0.5f -
                         std::abs(aCenterX - bCenterX);
        float overlapY = (colliderA.Height + colliderB.Height) * 0.5f -
                         std::abs((transformA.Y + colliderA.OffsetY + colliderA.Height * 0.5f) -
                                  (transformB.Y + colliderB.OffsetY + colliderB.Height * 0.5f));
        float overlapZ = (colliderA.Depth + colliderB.Depth) * 0.5f -
                         std::abs(aCenterZ - bCenterZ);

        if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f)
            return;

        bool aStatic = colliderA.IsStatic;
        bool bStatic = colliderB.IsStatic;

        // Push apart along the axis with the smallest overlap (minimum penetration)
        if (overlapX <= overlapY && overlapX <= overlapZ)
        {
            float sign = (aCenterX < bCenterX) ? -1.0f : 1.0f;

            if (aStatic && !bStatic)
                transformB.X -= sign * overlapX;
            else if (bStatic && !aStatic)
                transformA.X += sign * overlapX;
            else
            {
                transformA.X += sign * overlapX * 0.5f;
                transformB.X -= sign * overlapX * 0.5f;
            }
        }
        else if (overlapZ <= overlapY)
        {
            float sign = (aCenterZ < bCenterZ) ? -1.0f : 1.0f;

            if (aStatic && !bStatic)
                transformB.Z -= sign * overlapZ;
            else if (bStatic && !aStatic)
                transformA.Z += sign * overlapZ;
            else
            {
                transformA.Z += sign * overlapZ * 0.5f;
                transformB.Z -= sign * overlapZ * 0.5f;
            }
        }
        else
        {
            float centerAY = transformA.Y + colliderA.OffsetY + colliderA.Height * 0.5f;
            float centerBY = transformB.Y + colliderB.OffsetY + colliderB.Height * 0.5f;
            float sign = (centerAY < centerBY) ? -1.0f : 1.0f;

            if (aStatic && !bStatic)
                transformB.Y -= sign * overlapY;
            else if (bStatic && !aStatic)
                transformA.Y += sign * overlapY;
            else
            {
                transformA.Y += sign * overlapY * 0.5f;
                transformB.Y -= sign * overlapY * 0.5f;
            }
        }
    }
}
