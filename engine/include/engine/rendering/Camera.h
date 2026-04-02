// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERA_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERA_H

namespace engine
{
    class Camera
    {
    public:
        Camera();

        void SetPosition(float x, float y, float z);
        void SetTarget(float x, float y, float z);
        void SetPerspective(float fieldOfViewDegrees, float aspectRatio, float nearPlane, float farPlane);
        void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

        [[nodiscard]] const float* GetViewMatrix() const;
        [[nodiscard]] const float* GetProjectionMatrix() const;

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

    private:
        float m_Position[3];
        float m_Target[3];
        float m_Up[3];

        float m_ViewMatrix[16];
        float m_ProjectionMatrix[16];

        bool m_IsOrthographic;

        float m_FieldOfViewDegrees;
        float m_AspectRatio;
        float m_NearPlane;
        float m_FarPlane;

        float m_OrthoLeft;
        float m_OrthoRight;
        float m_OrthoBottom;
        float m_OrthoTop;
    };
}

#endif
