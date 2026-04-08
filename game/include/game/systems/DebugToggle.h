// Created by Elijah Fabon

// DebugToggle -- Manages debug overlay visibility via keyboard shortcuts.
// F3 toggles the master debug mode; F4 and F5 independently toggle the
// FPS counter and collider wireframe sub-features within debug mode.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGTOGGLE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGTOGGLE_H

#include "engine/input/Input.h"

namespace game
{
    class DebugToggle
    {
    public:
        void Update(const engine::Input& input);

        [[nodiscard]] bool IsEnabled() const { return m_Enabled; }
        [[nodiscard]] bool ShowFPS() const { return m_Enabled && m_ShowFPS; }
        [[nodiscard]] bool ShowColliders() const { return m_Enabled && m_ShowColliders; }

    private:
        bool m_Enabled = false;
        bool m_ShowFPS = true;
        bool m_ShowColliders = true;
        // Previous-frame key states for edge detection (press, not hold)
        bool m_F3WasDown = false;
        bool m_F4WasDown = false;
        bool m_F5WasDown = false;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGTOGGLE_H
