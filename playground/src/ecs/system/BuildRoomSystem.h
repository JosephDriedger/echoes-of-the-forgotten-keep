//
// Created by adamd on 2026-03-26.
//

#ifndef ECHOESOFTHEFORGOTTENKEEP_BUILDROOMSYSTEM_H
#define ECHOESOFTHEFORGOTTENKEEP_BUILDROOMSYSTEM_H

#include <vector>
#include <string>
#include "World.h"
#include "SpawnSystem.h"

// Logical representation (NOT rendering)
enum class CellType {
    Empty,
    Floor,
    Wall,
    Door
};

struct MapGrid {
    int width = 0;
    int height = 0;
    std::vector<CellType> cells;

    CellType& get(int x, int y) {
        return cells[y * width + x];
    }
};

class BuildRoomSystem {
public:

    // Generate a simple room (MVP)
    static MapGrid GenerateRoom(int width, int height);

    // Build ECS entities from grid
    static void Build(World& world, const MapGrid& map);

    // Save / Load
    static void Save(const MapGrid& map, const std::string& file);
    static MapGrid Load(const std::string& file);

private:
    static constexpr float TILE_SIZE = 4.0f;
    static constexpr float FLOOR_OFFSET = 2.0f;

    // Optional helper
    static bool RandomChance(float chance);
    static bool isWall(const MapGrid& map, int x, int y);
    static void getWallTypeAndRotation(const MapGrid& map, int x, int y,
                           AssetType& type, glm::vec3& rotation);
    static bool isInterior(int x, int y, const MapGrid& map);
};

#endif //ECHOESOFTHEFORGOTTENKEEP_BUILDROOMSYSTEM_H