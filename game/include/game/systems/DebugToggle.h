// Created by Elijah Fabon

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
        bool m_F3WasDown = false;
        bool m_F4WasDown = false;
        bool m_F5WasDown = false;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_DEBUGTOGGLE_H
