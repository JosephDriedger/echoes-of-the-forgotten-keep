#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_UISLIDER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_UISLIDER_H

#include <string>

namespace engine
{
    class TextRenderer;
    class QuadRenderer;
    class Input;
}

namespace game
{
    class UISlider
    {
    public:
        UISlider();
        UISlider(const std::string& label, float x, float y,
                 float trackWidth, float* valuePtr,
                 float minVal = 0.0f, float maxVal = 1.0f);

        void Update(const engine::Input& input);
        void Render(engine::TextRenderer& textRenderer,
                    engine::QuadRenderer& quadRenderer) const;

        void SetPosition(float x, float y);
        void CenterHorizontally(float screenWidth);

        float GetTotalWidth() const;

    private:
        std::string m_Label;
        float m_X, m_Y;
        float m_TrackWidth;
        float m_TrackHeight;
        float m_HandleWidth;
        float m_HandleHeight;
        float m_MinVal, m_MaxVal;
        float* m_ValuePtr;
        bool m_Dragging;

        float m_LabelWidth;
        float m_LabelOffsetX;
        float m_TrackOffsetX;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_UISLIDER_H
