//
// Created by scept on 2026-03-09.
//

#ifndef INC_8051TUTORIAL_DAMAGESYSTEM_H
#define INC_8051TUTORIAL_DAMAGESYSTEM_H

#pragma once

#include "engine/ecs/Registry.h"

namespace game
{
    class DamageSystem
    {
    public:
        static void Update(engine::Registry& registry);
    };
}

#endif //INC_8051TUTORIAL_DAMAGESYSTEM_H