// Created by Adam Van Woerden
/// @file UIButton.cpp
/// @brief Text button with AABB hover detection and single-frame click check.

#include "game/ui/UIButton.h"

#include "engine/rendering/TextRenderer.h"
#include "engine/input/Input.h"

namespace game
{
    UIButton::UIButton()
        : m_X(0), m_Y(0), m_Width(0), m_Height(0), m_Hovered(false)
    {
    }

    UIButton::UIButton(const std::string& label, float x, float y)
        : m_Label(label), m_X(x), m_Y(y), m_Width(0), m_Height(0), m_Hovered(false)
    {
    }

    void UIButton::Layout(engine::TextRenderer& textRenderer, float scale, float padding)
    {
        float textWidth = textRenderer.MeasureTextWidth(m_Label, scale);
        float textHeight = textRenderer.GetLineHeight(scale);

        m_Width = textWidth + padding * 2.0f;
        m_Height = textHeight + padding;
    }

    void UIButton::UpdateHover(int mouseX, int mouseY)
    {
        float mx = static_cast<float>(mouseX);
        float my = static_cast<float>(mouseY);

        m_Hovered = mx >= m_X && mx <= m_X + m_Width &&
                    my >= m_Y && my <= m_Y + m_Height;
    }

    bool UIButton::IsClicked(const engine::Input& input) const
    {
        return m_Hovered && input.IsMouseButtonPressed(1);
    }

    void UIButton::Render(engine::TextRenderer& textRenderer, float scale) const
    {
        float textWidth = textRenderer.MeasureTextWidth(m_Label, scale);
        float textX = m_X + (m_Width - textWidth) / 2.0f;
        float textY = m_Y + 15.0f;

        // Gold highlight on hover, white otherwise
        if (m_Hovered)
        {
            textRenderer.RenderText(m_Label, textX, textY, scale,
                                    1.0f, 0.85f, 0.4f);
        }
        else
        {
            textRenderer.RenderText(m_Label, textX, textY, scale,
                                    0.9f, 0.9f, 0.9f);
        }
    }

    void UIButton::SetPosition(float x, float y)
    {
        m_X = x;
        m_Y = y;
    }

    void UIButton::CenterHorizontally(float screenWidth)
    {
        m_X = (screenWidth - m_Width) / 2.0f;
    }
}
