#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_UIBUTTON_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_UIBUTTON_H

#include <string>

namespace engine
{
    class TextRenderer;
    class Input;
}

namespace game
{
    class UIButton
    {
    public:
        UIButton();
        UIButton(const std::string& label, float x, float y);

        void Layout(engine::TextRenderer& textRenderer, float scale = 1.0f, float padding = 30.0f);
        void UpdateHover(int mouseX, int mouseY);
        bool IsClicked(const engine::Input& input) const;

        void Render(engine::TextRenderer& textRenderer, float scale = 1.0f) const;

        const std::string& GetLabel() const { return m_Label; }
        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }
        float GetWidth() const { return m_Width; }
        float GetHeight() const { return m_Height; }
        bool IsHovered() const { return m_Hovered; }

        void SetPosition(float x, float y);
        void CenterHorizontally(float screenWidth);

    private:
        std::string m_Label;
        float m_X, m_Y;
        float m_Width, m_Height;
        bool m_Hovered;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_UIBUTTON_H
