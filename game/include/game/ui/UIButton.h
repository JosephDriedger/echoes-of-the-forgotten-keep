// Created by Adam Van Woerden
/// @file UIButton.h
/// @brief Reusable text button widget for menu screens.
///
/// A UIButton is a clickable text label with hover highlighting.
/// It does not own any GPU resources -- it delegates rendering to
/// a TextRenderer passed into Layout() and Render().
///
/// Typical usage:
///   UIButton btn("Play", x, y);
///   btn.Layout(textRenderer);         // measure text, compute bounds
///   btn.CenterHorizontally(screenW);  // optional centering
///   // each frame:
///   btn.UpdateHover(mouseX, mouseY);
///   if (btn.IsClicked(input)) { ... }
///   btn.Render(textRenderer);

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

        /// Measure the label text and compute the button's width/height.
        void Layout(engine::TextRenderer& textRenderer, float scale = 1.0f, float padding = 30.0f);

        /// Test whether the mouse is inside the button bounds.
        void UpdateHover(int mouseX, int mouseY);

        /// Returns true on the frame the left mouse button is pressed while hovered.
        bool IsClicked(const engine::Input& input) const;

        /// Draw the label, using gold for hovered and white for normal.
        void Render(engine::TextRenderer& textRenderer, float scale = 1.0f) const;

        const std::string& GetLabel() const { return m_Label; }
        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }
        float GetWidth() const { return m_Width; }
        float GetHeight() const { return m_Height; }
        bool IsHovered() const { return m_Hovered; }

        void SetPosition(float x, float y);

        /// Reposition the button so it is horizontally centered on screen.
        void CenterHorizontally(float screenWidth);

    private:
        std::string m_Label;
        float m_X, m_Y;
        float m_Width, m_Height;
        bool m_Hovered;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_UIBUTTON_H
