// Created by Adam Van Woerden

#include "game/ui/UISlider.h"

#include "engine/rendering/TextRenderer.h"
#include "engine/rendering/QuadRenderer.h"
#include "engine/input/Input.h"

#include <algorithm>
#include <string>

namespace game
{
    UISlider::UISlider()
        : m_X(0), m_Y(0),
          m_TrackWidth(300.0f), m_TrackHeight(8.0f),
          m_HandleWidth(16.0f), m_HandleHeight(28.0f),
          m_MinVal(0.0f), m_MaxVal(1.0f),
          m_ValuePtr(nullptr), m_Dragging(false),
          m_LabelWidth(0), m_LabelOffsetX(0), m_TrackOffsetX(0)
    {
    }

    UISlider::UISlider(const std::string& label, float x, float y,
                       float trackWidth, float* valuePtr,
                       float minVal, float maxVal)
        : m_Label(label), m_X(x), m_Y(y),
          m_TrackWidth(trackWidth), m_TrackHeight(8.0f),
          m_HandleWidth(16.0f), m_HandleHeight(28.0f),
          m_MinVal(minVal), m_MaxVal(maxVal),
          m_ValuePtr(valuePtr), m_Dragging(false),
          m_LabelWidth(0), m_LabelOffsetX(0), m_TrackOffsetX(0)
    {
    }

    void UISlider::Layout(engine::TextRenderer& textRenderer, float labelScale)
    {
        m_LabelWidth = textRenderer.MeasureTextWidth(m_Label, labelScale);
        m_TrackOffsetX = m_LabelWidth + 20.0f;
    }

    void UISlider::Update(const engine::Input& input)
    {
        if (!m_ValuePtr) return;

        float mx = static_cast<float>(input.GetMouseX());
        float my = static_cast<float>(input.GetMouseY());

        float trackX = m_X + m_TrackOffsetX;
        float trackCenterY = m_Y + m_HandleHeight / 2.0f;

        // Check if click is on track/handle area
        if (input.IsMouseButtonPressed(1))
        {
            if (mx >= trackX && mx <= trackX + m_TrackWidth &&
                my >= m_Y && my <= m_Y + m_HandleHeight)
            {
                m_Dragging = true;
            }
        }

        if (input.IsMouseButtonReleased(1))
        {
            m_Dragging = false;
        }

        if (m_Dragging && input.IsMouseButtonDown(1))
        {
            float normalized = (mx - trackX) / m_TrackWidth;
            normalized = std::clamp(normalized, 0.0f, 1.0f);
            *m_ValuePtr = m_MinVal + normalized * (m_MaxVal - m_MinVal);
        }
    }

    void UISlider::Render(engine::TextRenderer& textRenderer,
                          engine::QuadRenderer& quadRenderer) const
    {
        if (!m_ValuePtr) return;

        float labelScale = 0.6f;
        float trackX = m_X + m_TrackOffsetX;
        float trackCenterY = m_Y + m_HandleHeight / 2.0f;

        // Label
        textRenderer.RenderText(m_Label, m_X, m_Y - 5.0f, labelScale,
                                0.9f, 0.9f, 0.9f);

        // Track background
        quadRenderer.DrawFilledRect(trackX, trackCenterY - m_TrackHeight / 2.0f,
                                    m_TrackWidth, m_TrackHeight,
                                    0.3f, 0.3f, 0.3f, 1.0f);

        // Filled portion
        float normalized = (*m_ValuePtr - m_MinVal) / (m_MaxVal - m_MinVal);
        normalized = std::clamp(normalized, 0.0f, 1.0f);
        float filledWidth = m_TrackWidth * normalized;

        quadRenderer.DrawFilledRect(trackX, trackCenterY - m_TrackHeight / 2.0f,
                                    filledWidth, m_TrackHeight,
                                    0.85f, 0.65f, 0.3f, 1.0f);

        // Handle
        float handleX = trackX + filledWidth - m_HandleWidth / 2.0f;
        quadRenderer.DrawFilledRect(handleX, m_Y,
                                    m_HandleWidth, m_HandleHeight,
                                    0.95f, 0.95f, 0.95f, 1.0f);

        // Percentage text
        int percentage = static_cast<int>(normalized * 100.0f);
        std::string pctText = std::to_string(percentage) + "%";
        float pctX = trackX + m_TrackWidth + 15.0f;
        textRenderer.RenderText(pctText, pctX, m_Y - 5.0f, labelScale,
                                0.7f, 0.7f, 0.7f);
    }

    void UISlider::SetPosition(float x, float y)
    {
        m_X = x;
        m_Y = y;
    }

    void UISlider::CenterHorizontally(float screenWidth)
    {
        float totalWidth = m_TrackOffsetX + m_TrackWidth + 80.0f; // label + track + percentage
        m_X = (screenWidth - totalWidth) / 2.0f;
    }

    float UISlider::GetTotalWidth() const
    {
        return m_TrackOffsetX + m_TrackWidth + 80.0f;
    }
}
