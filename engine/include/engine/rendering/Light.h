//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_LIGHT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_LIGHT_H

namespace engine
{
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