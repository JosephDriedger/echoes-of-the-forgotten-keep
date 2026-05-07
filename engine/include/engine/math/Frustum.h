//
// Created by adamd on 2026-05-06.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_FRUSTUM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_FRUSTUM_H


#pragma once

#include <glm/glm.hpp>

namespace engine
{
    class Frustum
    {
    public:

        Frustum();

        // Extract planes from combined view-projection matrix
        void Update(const glm::mat4& vp);

        // Bounding sphere test (fast culling)
        bool IntersectsSphere(const glm::vec3& center, float radius) const;

        // Plane format: ax + by + cz + d = 0
        struct Plane
        {
            glm::vec3 normal;
            float d;
        };

    private:
        Plane m_Planes[6];

        enum
        {
            LEFT = 0,
            RIGHT,
            BOTTOM,
            TOP,
            NEAR,
            FAR
        };
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_FRUSTUM_H