// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SIGNATURE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SIGNATURE_H

#include <bitset>
#include <cstddef>

namespace engine
{
    // Upper bound on distinct component types that can be registered.
    constexpr std::size_t MAX_COMPONENTS = 64;

    // Bitset where each bit corresponds to a ComponentType.
    // An entity's signature indicates which components it currently holds,
    // and a system's signature indicates which components it requires.
    using Signature = std::bitset<MAX_COMPONENTS>;
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_SIGNATURE_H