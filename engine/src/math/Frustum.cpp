//
// Created by adamd on 2026-05-06.
//

#include "engine/math/Frustum.h"
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace engine {
    Frustum::Frustum()
    {
        for (auto& p : m_Planes)
        {
            p.normal = glm::vec3(0.0f);
            p.d = 0.0f;
        }
    }

    static void NormalizePlane(Frustum::Plane& p)
    {
        float length = glm::length(p.normal);
        p.normal /= length;
        p.d /= length;
    }

    void Frustum::Update(const glm::mat4& vp)
    {
        // LEFT
        m_Planes[LEFT].normal.x = vp[0][3] + vp[0][0];
        m_Planes[LEFT].normal.y = vp[1][3] + vp[1][0];
        m_Planes[LEFT].normal.z = vp[2][3] + vp[2][0];
        m_Planes[LEFT].d        = vp[3][3] + vp[3][0];
        NormalizePlane(m_Planes[LEFT]);

        // RIGHT
        m_Planes[RIGHT].normal.x = vp[0][3] - vp[0][0];
        m_Planes[RIGHT].normal.y = vp[1][3] - vp[1][0];
        m_Planes[RIGHT].normal.z = vp[2][3] - vp[2][0];
        m_Planes[RIGHT].d        = vp[3][3] - vp[3][0];
        NormalizePlane(m_Planes[RIGHT]);

        // BOTTOM
        m_Planes[BOTTOM].normal.x = vp[0][3] + vp[0][1];
        m_Planes[BOTTOM].normal.y = vp[1][3] + vp[1][1];
        m_Planes[BOTTOM].normal.z = vp[2][3] + vp[2][1];
        m_Planes[BOTTOM].d        = vp[3][3] + vp[3][1];
        NormalizePlane(m_Planes[BOTTOM]);

        // TOP
        m_Planes[TOP].normal.x = vp[0][3] - vp[0][1];
        m_Planes[TOP].normal.y = vp[1][3] - vp[1][1];
        m_Planes[TOP].normal.z = vp[2][3] - vp[2][1];
        m_Planes[TOP].d        = vp[3][3] - vp[3][1];
        NormalizePlane(m_Planes[TOP]);

        // NEAR
        m_Planes[NEAR].normal.x = vp[0][3] + vp[0][2];
        m_Planes[NEAR].normal.y = vp[1][3] + vp[1][2];
        m_Planes[NEAR].normal.z = vp[2][3] + vp[2][2];
        m_Planes[NEAR].d        = vp[3][3] + vp[3][2];
        NormalizePlane(m_Planes[NEAR]);

        // FAR
        m_Planes[FAR].normal.x = vp[0][3] - vp[0][2];
        m_Planes[FAR].normal.y = vp[1][3] - vp[1][2];
        m_Planes[FAR].normal.z = vp[2][3] - vp[2][2];
        m_Planes[FAR].d        = vp[3][3] - vp[3][2];
        NormalizePlane(m_Planes[FAR]);
    }

    bool Frustum::IntersectsSphere(const glm::vec3& center, float radius) const
    {
        for (const auto& plane : m_Planes)
        {
            float distance =
                glm::dot(plane.normal, center) + plane.d;

            if (distance < -radius)
                return false;
        }

        return true;
    }
}