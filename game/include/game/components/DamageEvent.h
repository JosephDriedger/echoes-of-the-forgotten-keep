//
// Created by adamd on 2026-03-31.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGEEVENT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGEEVENT_H
#include "engine/ecs/Entity.h"

struct DamageEvent {
    engine::Entity* target = nullptr;
    int amount = 1;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_DAMAGEEVENT_H