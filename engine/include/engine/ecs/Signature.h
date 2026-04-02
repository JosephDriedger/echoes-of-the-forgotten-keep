// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SIGNATURE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SIGNATURE_H

#include <bitset>
#include <cstddef>

namespace engine
{
    constexpr std::size_t MAX_COMPONENTS = 64;
    using Signature = std::bitset<MAX_COMPONENTS>;
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_SIGNATURE_H