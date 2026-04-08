// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_BUILDROOMSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_BUILDROOMSYSTEM_H

#pragma once

#include <vector>
#include <string>

#include "FloorGenerator.h"
#include "engine/resources/MeshManager.h"
#include "engine/scene/PrefabManager.h"

namespace engine {

    // 2D grid of CellType values stored in row-major order.
    // Used as the intermediate representation between procedural generation
    // (FloorGenerator) and 3D prefab placement (BuildRoomSystem::Build).
    struct MapGrid {
        int width = 0;
        int height = 0;
        std::vector<CellType> cells;

        CellType& get(int x, int y) {
            return cells[y * width + x];
        }
    };

    // Describes a single prefab to place in the 3D scene,
    // produced by BuildRoomSystem::Build from a MapGrid.
    struct SpawnInstance {
        PrefabType prefab;
        glm::vec3 position;
        float rotationY;
    };

    // Converts a 2D MapGrid into a list of 3D SpawnInstances (floors, walls, doors, stairs).
    // Also provides helpers to generate simple test rooms, convert FloorLayouts into
    // MapGrids with auto-detected walls and doors, and serialize/deserialize maps to disk.
    class BuildRoomSystem {
    public:
        // Creates a simple rectangular room with walls on the border and doors on north/south edges.
        static MapGrid GenerateRoom(int width, int height);

        // Walks every cell in the map and emits the corresponding SpawnInstances
        // (floor tiles, wall segments, doors, stairs) positioned in world space.
        static std::vector<SpawnInstance> Build(const MapGrid& map, const BuildRoomConfig& config);

        // Converts a raw FloorLayout (floors only) into a full MapGrid by:
        //   1. Copying floor/start/end cells
        //   2. Surrounding floors with walls (8-directional)
        //   3. Detecting door positions at corridor-to-room transitions
        static MapGrid FromFloor(const FloorLayout& floor);

        static void Save(const MapGrid& map, const std::string& file);
        static MapGrid Load(const std::string& file);
        static void DebugPrint(const MapGrid& map);

    private:
        static constexpr float TILE_SIZE = 4.0f;
        static constexpr float FLOOR_OFFSET = 2.0f;

        static bool RandomChance(float chance);
        static bool isWall(const MapGrid& map, int x, int y);
        // Counts cardinal (N/S/E/W) neighbors that are walkable (Floor, Start, or End).
        static int CountAdjacentFloors(MapGrid& map, int x, int y);
        // Returns true if the tile has 3+ adjacent floors, indicating it is inside a room area.
        static bool IsRoomTile(MapGrid& map, int x, int y);
        static bool isFloorLike(CellType c);

        // Determines the wall prefab type (straight, corner, T-split, crossing) and
        // Y-rotation based on which cardinal neighbors are also walls.
        static void getWallTypeAndRotation(
            const MapGrid& map,
            int x, int y,
            PrefabType& type,
            float& rotationY
        );

        static bool isInterior(int x, int y, const MapGrid& map);

        static PrefabType ConvertToPrefab(PrefabType type);

    };
}
#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_BUILDROOMSYSTEM_H