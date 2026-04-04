// Created by Joey Driedger

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

        bool Create(const char* title, int width, int height);
        void Destroy();
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