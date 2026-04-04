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
    struct MapGrid {
        int width = 0;
        int height = 0;
        std::vector<CellType> cells;

        CellType& get(int x, int y) {
            return cells[y * width + x];
        }
    };

    struct SpawnInstance {
        PrefabType prefab;
        glm::vec3 position;
        float rotationY;
    };

    class BuildRoomSystem {
    public:
        static MapGrid GenerateRoom(int width, int height);

        static std::vector<SpawnInstance> Build(const MapGrid& map, const BuildRoomConfig& config);
        static MapGrid FromFloor(const FloorLayout& floor);
        static void Save(const MapGrid& map, const std::string& file);
        static MapGrid Load(const std::string& file);
        static void DebugPrint(const MapGrid& map);

    private:
        static constexpr float TILE_SIZE = 4.0f;
        static constexpr float FLOOR_OFFSET = 2.0f;

        static bool RandomChance(float chance);
        static bool isWall(const MapGrid& map, int x, int y);
        static int CountAdjacentFloors(MapGrid& map, int x, int y);
        static bool IsRoomTile(MapGrid& map, int x, int y);
        static bool isFloorLike(CellType c);
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