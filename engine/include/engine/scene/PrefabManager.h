// Created by Joey Driedger
//
// Prefab registry: maps logical PrefabTypes to mesh/texture asset paths.
// Supports multiple weighted variants per type for visual variety (e.g.
// several wall meshes with different wear/damage).

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H
#include <string>
#include <unordered_map>
#include <vector>

namespace engine {

    // Logical prefab categories used by BuildRoomSystem when spawning dungeon geometry.
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

    // A single visual variant of a prefab: mesh path, texture path, and a
    // relative weight used for weighted-random selection.
    struct PrefabVariant {
        std::string meshPath;
        std::string texturePath;
        float probability = 1.0f; // relative weight, not a percentage
    };

    // All registered variants for a single PrefabType.
    struct PrefabDefinition {
        std::vector<PrefabVariant> variants;
    };

    // Global (static) registry of prefab definitions.
    // Game code registers variants at startup; BuildRoomSystem queries them at build time.
    class PrefabManager {
    public:
        // Adds a variant to the given prefab type. Multiple variants enable visual variety.
        static void Register(PrefabType type, const PrefabVariant& variant);
        // Returns a randomly chosen variant using weighted selection, or nullptr if none registered.
        static const PrefabVariant* GetRandom(PrefabType type);

    private:
        static std::unordered_map<PrefabType, PrefabDefinition> m_Definitions;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_PREFAB_H