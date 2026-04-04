// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H
#include <string>
#include <unordered_map>
#include <vector>

namespace engine {
    enum class PrefabType {
        Wall,
        WallCorner,
        WallCrossing,
        WallTsplit,
        WallDoorway,
        Door,
        FloorLarge,
        FloorSmall,
        Stairs,
        WallCornerSmall
    };

    struct PrefabVariant {
        std::string meshPath;
        std::string texturePath;
        float probability = 1.0f; // weight, not %
    };

    struct PrefabDefinition {
        std::vector<PrefabVariant> variants;
    };

    class PrefabManager {
    public:
        static void Register(PrefabType type, const PrefabVariant& variant);
        static const PrefabVariant* GetRandom(PrefabType type);

    private:
        static std::unordered_map<PrefabType, PrefabDefinition> m_Definitions;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H