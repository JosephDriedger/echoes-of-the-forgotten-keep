// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_LIGHT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_LIGHT_H

namespace engine
{
    // Represents a point light source with position, RGB color, and intensity.
    // Values are uploaded to the shader as uniforms for forward lighting calculations.
    class Light
    {
    public:
        Light();

        void SetPosition(float x, float y, float z);
        void SetColor(float r, float g, float b);
        void SetIntensity(float intensity);

        [[nodiscard]] const float* GetPosition() const;
        [[nodiscard]] const float* GetColor() const;
        [[nodiscard]] float GetIntensity() const;

    private:
        float m_Position[3];
        float m_Color[3];
        float m_Intensity;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_LIGHT_H