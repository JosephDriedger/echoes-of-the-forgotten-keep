// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H

#include <cstdint>

namespace engine
{
    // Numeric identifier assigned to each registered component type.
    // Used as the bit index within a Signature bitset to track which
    // components an entity possesses.
    using ComponentType = std::uint8_t;
    constexpr ComponentType INVALID_COMPONENT_TYPE = static_cast<ComponentType>(255);
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H