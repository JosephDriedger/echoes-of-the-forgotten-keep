//
// Created by adamd on 2026-03-31.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKREQUEST_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKREQUEST_H
#include <string>

#include "glm/vec3.hpp"

struct AttackRequest {
    // Vector2D direction{};
    glm::vec3 direction{};
    std::string tag;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ATTACKREQUEST_H