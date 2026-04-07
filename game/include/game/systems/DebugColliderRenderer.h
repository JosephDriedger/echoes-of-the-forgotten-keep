// Created by Elijah Fabon

// DebugColliderRenderer -- Draws wireframe boxes around every entity's
// collider using a minimal GL shader. Color-coded: green for the player,
// blue for triggers, yellow for normal colliders. Only active when the
// debug overlay is enabled.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGCOLLIDERRENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGCOLLIDERRENDERER_H

#include "engine/ecs/Registry.h"
#include "engine/rendering/Camera.h"

namespace game
{
    class DebugColliderRenderer
    {
    public:
        DebugColliderRenderer();
        ~DebugColliderRenderer();

        bool Initialize();
        void Render(const engine::Registry& registry, const engine::Camera& camera);

    private:
        unsigned int m_ShaderProgram = 0; // Simple flat-color line shader
        unsigned int m_VAO = 0;          // Unit-cube wireframe vertex array
        unsigned int m_VBO = 0;
        bool m_Initialized = false;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGCOLLIDERRENDERER_H
