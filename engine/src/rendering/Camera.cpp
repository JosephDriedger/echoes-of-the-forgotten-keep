#include "engine/rendering/Camera.h"

#include <cmath>

namespace engine
{
    namespace
    {
        void SetIdentity(float* matrix)
        {
            for (int index = 0; index < 16; ++index)
            {
                matrix[index] = 0.0f;
            }

            matrix[0] = 1.0f;
            matrix[5] = 1.0f;
            matrix[10] = 1.0f;
            matrix[15] = 1.0f;
        }

        void Normalize(float* vector)
        {
            const float length = std::sqrt(
                vector[0] * vector[0] +
                vector[1] * vector[1] +
                vector[2] * vector[2]);

            if (length <= 0.00001f)
            {
                return;
            }

            vector[0] /= length;
            vector[1] /= length;
            vector[2] /= length;
        }

        void Cross(const float* a, const float* b, float* result)
        {
            result[0] = a[1] * b[2] - a[2] * b[1];
            result[1] = a[2] * b[0] - a[0] * b[2];
            result[2] = a[0] * b[1] - a[1] * b[0];
        }

        float Dot(const float* a, const float* b)
        {
            return
                a[0] * b[0] +
                a[1] * b[1] +
                a[2] * b[2];
        }
    }

    Camera::Camera()
        : m_Position{0.0f, 0.0f, 3.0f},
          m_Target{0.0f, 0.0f, 0.0f},
          m_Up{0.0f, 1.0f, 0.0f},
          m_ViewMatrix{},
          m_ProjectionMatrix{},
          m_IsOrthographic(false),
          m_FieldOfViewDegrees(45.0f),
          m_AspectRatio(16.0f / 9.0f),
          m_NearPlane(0.1f),
          m_FarPlane(100.0f),
          m_OrthoLeft(-8.0f),
          m_OrthoRight(8.0f),
          m_OrthoBottom(-4.5f),
          m_OrthoTop(4.5f)
    {
        RecalculateViewMatrix();
        RecalculateProjectionMatrix();
    }

    void Camera::SetPosition(const float x, const float y, const float z)
    {
        m_Position[0] = x;
        m_Position[1] = y;
        m_Position[2] = z;
        RecalculateViewMatrix();
    }

    void Camera::SetTarget(const float x, const float y, const float z)
    {
        m_Target[0] = x;
        m_Target[1] = y;
        m_Target[2] = z;
        RecalculateViewMatrix();
    }

    void Camera::SetPerspective(
        const float fieldOfViewDegrees,
        const float aspectRatio,
        const float nearPlane,
        const float farPlane)
    {
        m_IsOrthographic = false;
        m_FieldOfViewDegrees = fieldOfViewDegrees;
        m_AspectRatio = aspectRatio;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        RecalculateProjectionMatrix();
    }

    void Camera::SetOrthographic(
        const float left,
        const float right,
        const float bottom,
        const float top,
        const float nearPlane,
        const float farPlane)
    {
        m_IsOrthographic = true;
        m_OrthoLeft = left;
        m_OrthoRight = right;
        m_OrthoBottom = bottom;
        m_OrthoTop = top;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        RecalculateProjectionMatrix();
    }

    const float* Camera::GetViewMatrix() const
    {
        return m_ViewMatrix;
    }

    const float* Camera::GetProjectionMatrix() const
    {
        return m_ProjectionMatrix;
    }

    void Camera::RecalculateViewMatrix()
    {
        float forward[3] =
        {
            m_Target[0] - m_Position[0],
            m_Target[1] - m_Position[1],
            m_Target[2] - m_Position[2]
        };
        Normalize(forward);

        float right[3] = {};
        Cross(forward, m_Up, right);
        Normalize(right);

        float up[3] = {};
        Cross(right, forward, up);

        SetIdentity(m_ViewMatrix);

        m_ViewMatrix[0] = right[0];
        m_ViewMatrix[4] = right[1];
        m_ViewMatrix[8] = right[2];

        m_ViewMatrix[1] = up[0];
        m_ViewMatrix[5] = up[1];
        m_ViewMatrix[9] = up[2];

        m_ViewMatrix[2] = -forward[0];
        m_ViewMatrix[6] = -forward[1];
        m_ViewMatrix[10] = -forward[2];

        m_ViewMatrix[12] = -Dot(right, m_Position);
        m_ViewMatrix[13] = -Dot(up, m_Position);
        m_ViewMatrix[14] = Dot(forward, m_Position);
    }

    void Camera::RecalculateProjectionMatrix()
    {
        for (float& value : m_ProjectionMatrix)
        {
            value = 0.0f;
        }

        if (m_IsOrthographic)
        {
            m_ProjectionMatrix[0] = 2.0f / (m_OrthoRight - m_OrthoLeft);
            m_ProjectionMatrix[5] = 2.0f / (m_OrthoTop - m_OrthoBottom);
            m_ProjectionMatrix[10] = -2.0f / (m_FarPlane - m_NearPlane);
            m_ProjectionMatrix[12] = -(m_OrthoRight + m_OrthoLeft) / (m_OrthoRight - m_OrthoLeft);
            m_ProjectionMatrix[13] = -(m_OrthoTop + m_OrthoBottom) / (m_OrthoTop - m_OrthoBottom);
            m_ProjectionMatrix[14] = -(m_FarPlane + m_NearPlane) / (m_FarPlane - m_NearPlane);
            m_ProjectionMatrix[15] = 1.0f;
        }
        else
        {
            const float radians = m_FieldOfViewDegrees * 3.14159265358979323846f / 180.0f;
            const float tangent = std::tan(radians / 2.0f);

            m_ProjectionMatrix[0] = 1.0f / (m_AspectRatio * tangent);
            m_ProjectionMatrix[5] = 1.0f / tangent;
            m_ProjectionMatrix[10] = -(m_FarPlane + m_NearPlane) / (m_FarPlane - m_NearPlane);
            m_ProjectionMatrix[11] = -1.0f;
            m_ProjectionMatrix[14] = -(2.0f * m_FarPlane * m_NearPlane) / (m_FarPlane - m_NearPlane);
        }
    }
}
