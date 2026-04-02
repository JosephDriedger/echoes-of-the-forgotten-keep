// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_INPUT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_INPUT_H

#include <SDL3/SDL.h>

#include <unordered_set>

namespace engine
{
    class Input
    {
    public:
        Input();

        void BeginFrame();
        void ProcessEvent(const SDL_Event& event);
        void Reset();

        bool IsKeyDown(SDL_Keycode key) const;
        bool IsKeyPressed(SDL_Keycode key) const;
        bool IsKeyReleased(SDL_Keycode key) const;

        bool IsMouseButtonDown(Uint8 button) const;
        bool IsMouseButtonPressed(Uint8 button) const;
        bool IsMouseButtonReleased(Uint8 button) const;

        int GetMouseX() const;
        int GetMouseY() const;
        float GetMouseDeltaX() const;
        float GetMouseDeltaY() const;
        float GetMouseWheelX() const;
        float GetMouseWheelY() const;

        bool IsQuitRequested() const;

    private:
        std::unordered_set<SDL_Keycode> m_CurrentKeys;
        std::unordered_set<SDL_Keycode> m_PreviousKeys;
        std::unordered_set<Uint8> m_CurrentMouseButtons;
        std::unordered_set<Uint8> m_PreviousMouseButtons;

        int m_MouseX;
        int m_MouseY;
        float m_MouseDeltaX;
        float m_MouseDeltaY;
        float m_MouseWheelX;
        float m_MouseWheelY;
        bool m_QuitRequested;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_INPUT_H