// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERA_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_CAMERA_H

namespace engine
{
    // Look-at camera supporting both perspective and orthographic projection.
    // Stores raw float[16] matrices in column-major order (OpenGL convention).
    // View and projection matrices are recalculated automatically on parameter change.
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

        // Accessors used for distance-based culling in the render loop.
        [[nodiscard]] float GetPositionX() const { return m_Position[0]; }
        [[nodiscard]] float GetPositionY() const { return m_Position[1]; }
        [[nodiscard]] float GetPositionZ() const { return m_Position[2]; }

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

    private:
        float m_Position[3];  // Eye position in world space
        float m_Target[3];    // Look-at target in world space
        float m_Up[3];        // World up vector (typically +Y)

        float m_ViewMatrix[16];       // Column-major 4x4 view matrix
        float m_ProjectionMatrix[16]; // Column-major 4x4 projection matrix

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
