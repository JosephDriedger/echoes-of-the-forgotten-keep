//
// Created by Joseph Driedger on 3/8/2026.
//

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

        float m_FieldOfViewDegrees;
        float m_AspectRatio;
        float m_NearPlane;
        float m_FarPlane;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERA_H