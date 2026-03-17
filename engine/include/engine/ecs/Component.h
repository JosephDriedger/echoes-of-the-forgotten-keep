//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H

#include <cstdint>

namespace engine
{
    using ComponentType = std::uint8_t;
    constexpr ComponentType INVALID_COMPONENT_TYPE = static_cast<ComponentType>(255);
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H