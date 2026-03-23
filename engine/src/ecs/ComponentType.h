//
// Created by adamd on 2026-01-21.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENTTYPE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENTTYPE_H
#include <cstddef>

// creating a type alias for std:size_t
// size_t is designed to represent sizes and indices because its unsigned, and it is very (long, long).
using ComponentTypeID = std::size_t;

// when you have a free function and defined in a header, each .cpp file that includes this header (ComponentType.h)
// would get its own definition causing a linker error, using inline merges them into one definition
// global counter generator, returns a unique number everytime we call it
// making id static , means it will keep its value trough multiple calls of this function
inline ComponentTypeID getComponentTypeID() {
    static ComponentTypeID id = 0;
    return id++;
}

// can overload this function because they have different signatures
// template allows u to use generic types
// template is inline by default
// getComponentTypeID<Position>() -> would always return 0
// getComponentTypeID<Health>() -> would always return 1
// a static local variable is created an initialized once
template <typename T>
ComponentTypeID getComponentTypeID() {
    static ComponentTypeID typeID = getComponentTypeID();
    return typeID;
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENTTYPE_H