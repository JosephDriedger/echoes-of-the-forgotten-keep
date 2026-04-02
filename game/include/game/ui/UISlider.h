/// @file UISlider.h
/// @brief Reusable horizontal slider widget for settings screens.
///
/// A UISlider displays a labeled track with a draggable handle.
/// It writes directly to a float* (typically a field in GameSettings)
/// so no callback wiring is needed. The track is drawn with QuadRenderer
/// and the label/percentage are drawn with TextRenderer.
///
/// Typical usage:
///   UISlider slider("Music", x, y, 300, &settings.MusicVolume);
///   slider.Layout(textRenderer);            // measure label width
///   slider.CenterHorizontally(screenWidth); // optional centering
///   // each frame:
///   slider.Update(input);                   // handle drag interaction
///   slider.Render(textRenderer, quadRenderer);

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

        /// Measure label text width to compute track offset. Must be called
        /// after construction and before Render/CenterHorizontally.
        void Layout(engine::TextRenderer& textRenderer, float labelScale = 0.6f);

        /// Handle mouse click and drag interaction. Writes to *valuePtr.
        void Update(const engine::Input& input);

        /// Draw the label, track, filled portion, handle, and percentage text.
        void Render(engine::TextRenderer& textRenderer,
                    engine::QuadRenderer& quadRenderer) const;

        void SetPosition(float x, float y);
        void CenterHorizontally(float screenWidth);
        float GetTotalWidth() const;

    private:
        std::string m_Label;
        float m_X, m_Y;           ///< Top-left position of the entire widget
        float m_TrackWidth;        ///< Width of the slider track in pixels
        float m_TrackHeight;       ///< Height of the slider track bar
        float m_HandleWidth;       ///< Width of the draggable handle
        float m_HandleHeight;      ///< Height of the draggable handle
        float m_MinVal, m_MaxVal;  ///< Value range the slider maps to
        float* m_ValuePtr;         ///< Points directly into a GameSettings field
        bool m_Dragging;           ///< True while the user is dragging the handle

        float m_LabelWidth;        ///< Measured width of the label text
        float m_LabelOffsetX;      ///< Unused (reserved)
        float m_TrackOffsetX;      ///< Horizontal offset from m_X to the track start
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_UISLIDER_H
