// Created by Joey Driedger

// Window -- owns the SDL3 window and its associated OpenGL 4.6 core context.
// Create() sets up the window, GL context, and loads GL function pointers
// via GLAD. Destroy() tears everything down in the correct order.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_WINDOW_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_WINDOW_H

#include <SDL3/SDL.h>

namespace engine
{
    class Window
    {
    public:
        Window();
        ~Window();

        // Creates the SDL window and OpenGL context. Returns false on failure.
        bool Create(const char* title, int width, int height);
        void Destroy();
        // Presents the back buffer (double-buffered swap).
        void SwapBuffers() const;

        SDL_Window* GetNativeWindow() const
        {
            return m_Window;
        }

    private:
        SDL_Window* m_Window = nullptr;
        SDL_GLContext m_Context = nullptr;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_WINDOW_H