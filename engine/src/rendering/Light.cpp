// Created by Adam Van Woerden
/// @file Light.cpp
/// @brief Point light data container. Position, color, and intensity values are
///        stored as raw floats for direct upload to shader uniforms.

#include "engine/rendering/Light.h"

namespace engine
{
    Light::Light()
        : m_Position{0.0f, 0.0f, 0.0f},
          m_Color{1.0f, 1.0f, 1.0f},
          m_Intensity(1.0f)
    {
    }

    void Light::SetPosition(const float x, const float y, const float z)
    {
        m_Position[0] = x;
        m_Position[1] = y;
        m_Position[2] = z;
    }

    void Light::SetColor(const float r, const float g, const float b)
    {
        m_Color[0] = r;
        m_Color[1] = g;
        m_Color[2] = b;
    }

    void Light::SetIntensity(const float intensity)
    {
        m_Intensity = intensity;
    }

    const float* Light::GetPosition() const
    {
        return m_Position;
    }

    const float* Light::GetColor() const
    {
        return m_Color;
    }

    float Light::GetIntensity() const
    {
        return m_Intensity;
    }
}