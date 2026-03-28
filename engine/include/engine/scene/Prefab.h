//
// Created by adamd on 2026-03-28.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H
#include <string>
#include <unordered_map>

namespace engine {
    enum class PrefabType {
        Wall,
        WallCorner,
        WallCrossing,
        WallTsplit,
        WallDoorwayScaffold,
        Door,
        FloorLarge,
        FloorSmall
    };

    struct PrefabDefinition {
        std::string meshPath;
        std::string texturePath;
    };

    class PrefabManager {
    public:
        static void Register(PrefabType type, const PrefabDefinition& def);
        static const PrefabDefinition* Get(PrefabType type);

    private:
        static std::unordered_map<PrefabType, PrefabDefinition> m_Definitions;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H