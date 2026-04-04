// Created by Joey Driedger

#include "engine/platform/Window.h"

#include <glad/gl.h>
#include <iostream>

namespace engine
{
    Window::Window()
    {
    }

    Window::~Window()
    {
        Destroy();
    }

    bool Window::Create(const char* title, int width, int height)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        m_Window = SDL_CreateWindow(
            title,
            width,
            height,
            SDL_WINDOW_OPENGL
        );

        if (!m_Window)
        {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }

        m_Context = SDL_GL_CreateContext(m_Window);

        if (!m_Context)
        {
            std::cerr << "OpenGL context failed: " << SDL_GetError() << std::endl;
            return false;
        }

        if (!SDL_GL_MakeCurrent(m_Window, m_Context))
        {
            std::cerr << "Failed to make OpenGL context current: " << SDL_GetError() << std::endl;
            return false;
        }

        if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        if (!SDL_GL_SetSwapInterval(1))
        {
            std::cerr << "Warning: Failed to enable VSync: " << SDL_GetError() << std::endl;
        }

        std::cout << "OpenGL Version: "
                  << reinterpret_cast<const char*>(glGetString(GL_VERSION))
                  << std::endl;

        return true;
    }

    void Window::Destroy()
    {
        if (m_Context)
        {
            SDL_GL_DestroyContext(m_Context);
            m_Context = nullptr;
        }

        if (m_Window)
        {
            SDL_DestroyWindow(m_Window);
            m_Window = nullptr;
        }
    }

    void Window::SwapBuffers() const
    {
        if (m_Window)
        {
            SDL_GL_SwapWindow(m_Window);
        }
    }
}