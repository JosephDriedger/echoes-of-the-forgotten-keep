// Created by Elijah Fabon

#include "game/systems/DebugToggle.h"

#include <SDL3/SDL_keycode.h>
#include <iostream>

namespace game
{
    void DebugToggle::Update(const engine::Input& input)
    {
        // F3 toggles debug mode on/off
        bool f3Down = input.IsKeyDown(SDLK_F3);
        if (f3Down && !m_F3WasDown)
        {
            m_Enabled = !m_Enabled;
            std::cout << "[Debug] Debug mode " << (m_Enabled ? "ON" : "OFF") << "\n";
        }
        m_F3WasDown = f3Down;

        // F4 toggles FPS display (within debug mode)
        bool f4Down = input.IsKeyDown(SDLK_F4);
        if (f4Down && !m_F4WasDown)
        {
            m_ShowFPS = !m_ShowFPS;
            std::cout << "[Debug] FPS display " << (m_ShowFPS ? "ON" : "OFF") << "\n";
        }
        m_F4WasDown = f4Down;

        // F5 toggles collider rendering (within debug mode)
        bool f5Down = input.IsKeyDown(SDLK_F5);
        if (f5Down && !m_F5WasDown)
        {
            m_ShowColliders = !m_ShowColliders;
            std::cout << "[Debug] Collider rendering " << (m_ShowColliders ? "ON" : "OFF") << "\n";
        }
        m_F5WasDown = f5Down;
    }
}
