//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/rendering/Renderer.h"
#include "engine/platform/Window.h"

namespace engine
{
    Renderer::Renderer()
        : m_Window(nullptr),
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
        m_IsInitialized = true;
        return true;
    }

    void Renderer::Shutdown()
    {
        m_Window = nullptr;
        m_IsInitialized = false;
    }

    void Renderer::BeginFrame()
    {
        if (!m_IsInitialized)
        {
            return;
        }
    }

    void Renderer::EndFrame()
    {
        if (!m_IsInitialized || !m_Window)
        {
            return;
        }

        m_Window->SwapBuffers();
    }

    bool Renderer::IsInitialized() const
    {
        return m_IsInitialized;
    }
}