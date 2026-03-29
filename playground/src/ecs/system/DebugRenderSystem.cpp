//
// Created by scept on 2026-03-25.
//

#include "DebugRenderSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void DebugRenderSystem::render(SDL_Renderer* renderer,
                              const std::vector<std::unique_ptr<Entity>>& entities,
                              int screenWidth,
                              int screenHeight)
{
    // --- find camera ---
    Entity* cameraEntity = nullptr;

    for (auto& e : entities) {
        if (e->hasComponent<Camera3D>() &&
            e->hasComponent<Transform3D>())
        {
            cameraEntity = e.get();
            break;
        }
    }

    if (!cameraEntity) return;

    auto& cam = cameraEntity->getComponent<Camera3D>();
    auto& camTransform = cameraEntity->getComponent<Transform3D>();

    // --- build matrices ---
    glm::mat4 view = glm::lookAt(
        camTransform.position,
        camTransform.position + cam.forward,
        cam.up
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(cam.fov),
        cam.aspectRatio,
        cam.nearPlane,
        cam.farPlane
    );

    // --- draw all colliders ---
    for (auto& e : entities) {

        if (!e->hasComponent<Transform3D>() ||
            !e->hasComponent<Collider3D>())
            continue;

        auto& t = e->getComponent<Transform3D>();
        auto& c = e->getComponent<Collider3D>();

        // optional: only draw active weapons
        if (e->hasComponent<ProjectileTag>()) {
            auto& weapon = e->getComponent<ProjectileTag>();
            // if (!weapon.active) continue;
        }

        // color coding
        if (e->hasComponent<PlayerTag>())
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        else if (e->hasComponent<EnemyTag>())
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        else
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        drawCollider(renderer, t, c, view, projection, screenWidth, screenHeight);
    }
}

glm::vec2 DebugRenderSystem::worldToScreen(const glm::vec3& pos,
                                           const glm::mat4& view,
                                           const glm::mat4& projection,
                                           int screenWidth,
                                           int screenHeight)
{
    glm::vec4 clip = projection * view * glm::vec4(pos, 1.0f);

    if (clip.w <= 0.0f)
        return {-1000, -1000}; // behind camera

    glm::vec3 ndc = glm::vec3(clip) / clip.w;

    glm::vec2 screen;
    screen.x = (ndc.x * 0.5f + 0.5f) * screenWidth;
    screen.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight;

    return screen;
}

void DebugRenderSystem::drawCollider(SDL_Renderer* renderer,
                                     const Transform3D& t,
                                     const Collider3D& c,
                                     const glm::mat4& view,
                                     const glm::mat4& projection,
                                     int screenWidth,
                                     int screenHeight)
{
    glm::vec3 min = t.position;
    glm::vec3 max = t.position + glm::vec3(c.Width, c.Height, c.Depth);

    glm::vec3 corners[8] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, max.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };

    glm::vec2 projected[8];

    for (int i = 0; i < 8; i++) {
        projected[i] = worldToScreen(corners[i], view, projection, screenWidth, screenHeight);
    }

    auto drawLine = [&](int a, int b) {
        SDL_RenderLine(renderer,
            (int)projected[a].x, (int)projected[a].y,
            (int)projected[b].x, (int)projected[b].y);
    };

    // bottom
    drawLine(0,1); drawLine(1,2); drawLine(2,3); drawLine(3,0);

    // top
    drawLine(4,5); drawLine(5,6); drawLine(6,7); drawLine(7,4);

    // verticals
    drawLine(0,4); drawLine(1,5); drawLine(2,6); drawLine(3,7);
}