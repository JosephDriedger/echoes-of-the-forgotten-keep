//
// Created by adamd on 2026-03-03.
//

#include "Animation3D.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

Animation3D::Animation3D(const std::string& animationPath)
{
    m_Scene = m_Importer.ReadFile(
        animationPath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs
    );

    if (!m_Scene || !m_Scene->mRootNode)
    {
        std::cout << "Failed to load animation: "
                  << m_Importer.GetErrorString() << std::endl;
        return;
    }

    if (m_Scene->mNumAnimations > 0)
    {
        aiAnimation* anim = m_Scene->mAnimations[0];

        m_Duration = static_cast<float>(anim->mDuration);
        m_TicksPerSecond = static_cast<float>(
            anim->mTicksPerSecond != 0 ?
            anim->mTicksPerSecond : 25.0f
        );
    }
}