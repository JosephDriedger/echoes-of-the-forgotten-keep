//
// Created by adamd on 2026-03-28.
//

//
// Created by adamd on 2026-03-28.
//

#include "engine/scene/Prefab.h"

#include <iostream>

namespace engine {

    std::unordered_map<PrefabType, PrefabDefinition> PrefabManager::m_Definitions;

    void PrefabManager::Register(PrefabType type, const PrefabDefinition& def) {

        if (m_Definitions.find(type) != m_Definitions.end()) {
            std::cerr << "[PrefabManager] Prefab already registered\n";
            return;
        }

        m_Definitions[type] = def;
    }

    const PrefabDefinition* PrefabManager::Get(PrefabType type) {

        auto it = m_Definitions.find(type);

        if (it == m_Definitions.end()) {
            std::cerr << "[PrefabManager] Prefab not found\n";
            return nullptr;
        }

        return &it->second;
    }

}