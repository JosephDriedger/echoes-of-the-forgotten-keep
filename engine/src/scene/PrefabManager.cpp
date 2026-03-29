//
// Created by adamd on 2026-03-28.
//

#include "engine/scene/PrefabManager.h"

#include <iostream>

namespace engine {

    std::unordered_map<PrefabType, PrefabDefinition> PrefabManager::m_Definitions;

    void PrefabManager::Register(PrefabType type, const PrefabVariant& variant) {
        m_Definitions[type].variants.push_back(variant);
    }

    const PrefabVariant* PrefabManager::GetRandom(PrefabType type) {
        auto it = m_Definitions.find(type);
        if (it == m_Definitions.end() || it->second.variants.empty())
            return nullptr;

        const auto& variants = it->second.variants;

        float totalWeight = 0.0f;
        for (const auto& v : variants)
            totalWeight += v.probability;

        float r = static_cast<float>(rand()) / RAND_MAX * totalWeight;

        for (const auto& v : variants) {
            r -= v.probability;
            if (r <= 0.0f)
                return &v;
        }

        return &variants.back();
    }
}