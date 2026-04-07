// Created by Joey Driedger

// Input -- frame-based input manager wrapping SDL3 keyboard and mouse.
// Tracks current and previous frame key/button state so callers can
// distinguish between held (Down), just-pressed (Pressed), and
// just-released (Released) inputs.

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

        // Must be called at the start of each frame to snapshot previous
        // state and reset per-frame accumulators (mouse delta, wheel, etc.).
        void BeginFrame();
        // Routes an SDL event to the appropriate internal handler.
        void ProcessEvent(const SDL_Event& event);
        // Clears all tracked state (useful on scene transitions).
        void Reset();

        // Returns true while the key is held down.
        bool IsKeyDown(SDL_Keycode key) const;
        // Returns true only on the frame the key was first pressed.
        bool IsKeyPressed(SDL_Keycode key) const;
        // Returns true only on the frame the key was released.
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
        // Two-frame key/button state used for edge detection (pressed/released).
        std::unordered_set<SDL_Keycode> m_CurrentKeys;
        std::unordered_set<SDL_Keycode> m_PreviousKeys;
        std::unordered_set<Uint8> m_CurrentMouseButtons;
        std::unordered_set<Uint8> m_PreviousMouseButtons;

        int m_MouseX;
        int m_MouseY;
        // Accumulated mouse motion delta for the current frame.
        float m_MouseDeltaX;
        float m_MouseDeltaY;
        // Accumulated scroll wheel delta for the current frame.
        float m_MouseWheelX;
        float m_MouseWheelY;
        bool m_QuitRequested;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_INPUT_H