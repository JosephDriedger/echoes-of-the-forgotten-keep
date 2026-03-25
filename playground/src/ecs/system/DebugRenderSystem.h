//
// Created by scept on 2026-03-24.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGRENDERSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGRENDERSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"
#include "SDL3/SDL_render.h"

class DebugRenderSystem {
public:
    void render(SDL_Renderer* renderer,
                const std::vector<std::unique_ptr<Entity>>& entities,
                int screenWidth,
                int screenHeight);
private:
    glm::vec2 worldToScreen(const glm::vec3& pos,
                            const glm::mat4& view,
                            const glm::mat4& projection,
                            int screenWidth,
                            int screenHeight);

    void drawCollider(SDL_Renderer* renderer,
                      const Transform3D& t,
                      const Collider3D& c,
                      const glm::mat4& view,
                      const glm::mat4& projection,
                      int screenWidth,
                      int screenHeight);
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGRENDERSYSTEM_H