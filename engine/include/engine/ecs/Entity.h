// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITY_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITY_H

#include <cstdint>

namespace engine
{
    using EntityId = std::uint32_t;

    constexpr EntityId INVALID_ENTITY_ID = 0;
    // Valid entity IDs range from MIN_ENTITY_ID to MAX_ENTITIES (inclusive).
    // ID 0 is reserved as the invalid/null sentinel so arrays indexed by
    // EntityId can treat index 0 as unused.
    constexpr EntityId MIN_ENTITY_ID = 1;
    constexpr EntityId MAX_ENTITIES = 5000;

    // Lightweight handle to an entity in the ECS. Wraps a numeric EntityId
    // and provides validity checks and comparison operators.
    // Entities are created and destroyed through the Registry.
    class Entity
    {
    public:
        Entity();
        explicit Entity(EntityId id);

        [[nodiscard]] EntityId GetId() const;
        [[nodiscard]] bool IsValid() const;

        // Allows use in boolean contexts (e.g. if-statements) for null checks.
        explicit operator bool() const;

        bool operator==(const Entity& other) const;
        bool operator!=(const Entity& other) const;
        // Less-than enables use in ordered containers like std::set.
        bool operator<(const Entity& other) const;

    private:
        EntityId m_Id;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITY_H