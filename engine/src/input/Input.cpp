// Created by Joey Driedger

#include "engine/input/Input.h"

namespace engine
{
    Input::Input()
        : m_MouseX(0),
          m_MouseY(0),
          m_MouseDeltaX(0.0f),
          m_MouseDeltaY(0.0f),
          m_MouseWheelX(0.0f),
          m_MouseWheelY(0.0f),
          m_QuitRequested(false)
    {
    }

    void Input::BeginFrame()
    {
        m_PreviousKeys = m_CurrentKeys;
        m_PreviousMouseButtons = m_CurrentMouseButtons;
        m_MouseDeltaX = 0.0f;
        m_MouseDeltaY = 0.0f;
        m_MouseWheelX = 0.0f;
        m_MouseWheelY = 0.0f;
        m_QuitRequested = false;
    }

    void Input::ProcessEvent(const SDL_Event& event)
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                m_QuitRequested = true;
                break;

            case SDL_EVENT_KEY_DOWN:
                // Ignore OS key-repeat events so IsKeyPressed fires once.
                if (!event.key.repeat)
                {
                    m_CurrentKeys.insert(event.key.key);
                }
                break;

            case SDL_EVENT_KEY_UP:
                m_CurrentKeys.erase(event.key.key);
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                m_CurrentMouseButtons.insert(event.button.button);
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                m_CurrentMouseButtons.erase(event.button.button);
                break;

            case SDL_EVENT_MOUSE_MOTION:
                m_MouseX = static_cast<int>(event.motion.x);
                m_MouseY = static_cast<int>(event.motion.y);
                // Accumulate relative motion; multiple events may arrive per frame.
                m_MouseDeltaX += event.motion.xrel;
                m_MouseDeltaY += event.motion.yrel;
                break;

            case SDL_EVENT_MOUSE_WHEEL:
                m_MouseWheelX += event.wheel.x;
                m_MouseWheelY += event.wheel.y;
                break;

            default:
                break;
        }
    }

    void Input::Reset()
    {
        m_CurrentKeys.clear();
        m_PreviousKeys.clear();
        m_CurrentMouseButtons.clear();
        m_PreviousMouseButtons.clear();
        m_MouseX = 0;
        m_MouseY = 0;
        m_MouseDeltaX = 0.0f;
        m_MouseDeltaY = 0.0f;
        m_MouseWheelX = 0.0f;
        m_MouseWheelY = 0.0f;
        m_QuitRequested = false;
    }

    bool Input::IsKeyDown(SDL_Keycode key) const
    {
        return m_CurrentKeys.find(key) != m_CurrentKeys.end();
    }

    bool Input::IsKeyPressed(SDL_Keycode key) const
    {
        return m_CurrentKeys.find(key) != m_CurrentKeys.end() &&
               m_PreviousKeys.find(key) == m_PreviousKeys.end();
    }

    bool Input::IsKeyReleased(SDL_Keycode key) const
    {
        return m_CurrentKeys.find(key) == m_CurrentKeys.end() &&
               m_PreviousKeys.find(key) != m_PreviousKeys.end();
    }

    bool Input::IsMouseButtonDown(Uint8 button) const
    {
        return m_CurrentMouseButtons.find(button) != m_CurrentMouseButtons.end();
    }

    bool Input::IsMouseButtonPressed(Uint8 button) const
    {
        return m_CurrentMouseButtons.find(button) != m_CurrentMouseButtons.end() &&
               m_PreviousMouseButtons.find(button) == m_PreviousMouseButtons.end();
    }

    bool Input::IsMouseButtonReleased(Uint8 button) const
    {
        return m_CurrentMouseButtons.find(button) == m_CurrentMouseButtons.end() &&
               m_PreviousMouseButtons.find(button) != m_PreviousMouseButtons.end();
    }

    int Input::GetMouseX() const
    {
        return m_MouseX;
    }

    int Input::GetMouseY() const
    {
        return m_MouseY;
    }

    float Input::GetMouseDeltaX() const
    {
        return m_MouseDeltaX;
    }

    float Input::GetMouseDeltaY() const
    {
        return m_MouseDeltaY;
    }

    float Input::GetMouseWheelX() const
    {
        return m_MouseWheelX;
    }

    float Input::GetMouseWheelY() const
    {
        return m_MouseWheelY;
    }

    bool Input::IsQuitRequested() const
    {
        return m_QuitRequested;
    }
}