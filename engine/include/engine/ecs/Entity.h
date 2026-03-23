//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITY_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITY_H

#include <cstdint>

namespace engine
{
    using EntityId = std::uint32_t;

    constexpr EntityId INVALID_ENTITY_ID = 0;
    constexpr EntityId MIN_ENTITY_ID = 1;
    constexpr EntityId MAX_ENTITIES = 5000;

    class Entity
    {
    public:
        Entity();
        explicit Entity(EntityId id);

        [[nodiscard]] EntityId GetId() const;
        [[nodiscard]] bool IsValid() const;

        explicit operator bool() const;

        bool operator==(const Entity& other) const;
        bool operator!=(const Entity& other) const;
        bool operator<(const Entity& other) const;

    private:
        EntityId m_Id;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ENTITY_H