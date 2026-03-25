//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/rendering/Renderer.h"

#include "engine/platform/Window.h"

#include <glad/gl.h>

namespace engine
{
    Renderer::Renderer()
        : m_Window(nullptr),
          m_ActiveCamera(nullptr),
          m_IsInitialized(false)
    {
    }

    Renderer::~Renderer()
    {
        Shutdown();
    }

    bool Renderer::Initialize(Window& window)
    {
        if (m_IsInitialized)
        {
            return true;
        }

        m_Window = &window;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        m_IsInitialized = true;
        return true;
    }

    void Renderer::Shutdown()
    {
        m_Window = nullptr;
        m_ActiveCamera = nullptr;
        m_IsInitialized = false;
    }

    void Renderer::BeginFrame()
    {
        if (!m_IsInitialized)
        {
            return;
        }

        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::EndFrame()
    {
        if (!m_IsInitialized || !m_Window)
        {
            return;
        }

        m_Window->SwapBuffers();
    }

    void Renderer::SetActiveCamera(const Camera* camera)
    {
        m_ActiveCamera = camera;
    }

    const Camera* Renderer::GetActiveCamera() const
    {
        return m_ActiveCamera;
    }

    bool Renderer::IsInitialized() const
    {
        return m_IsInitialized;
    }
}