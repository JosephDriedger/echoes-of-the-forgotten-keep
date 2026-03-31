//
// Created by adamd on 2026-03-26.
//

#include "BuildRoomSystem.h"

#include <fstream>
#include <random>
#include <iostream>

bool BuildRoomSystem::RandomChance(float chance) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng) < chance;
}

MapGrid BuildRoomSystem::GenerateRoom(int width, int height) {

    MapGrid map;
    map.width = width;
    map.height = height;
    map.cells.resize(width * height, CellType::Empty);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            if (x == 0 || y == 0 || x == width - 1 || y == height - 1) {
                map.get(x, y) = CellType::Wall;
            } else {
                map.get(x, y) = CellType::Floor;
            }
        }
    }

    // Add doors (randomized)
    if (RandomChance(1.0f)) // always for now
        map.get(width / 2, 0) = CellType::Door;

    if (RandomChance(1.0f))
        map.get(width / 2, height - 1) = CellType::Door;

    return map;
}

void BuildRoomSystem::Build(World& world, const MapGrid& map) {

    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {

            CellType cell = map.cells[y * map.width + x];

            glm::vec3 pos = {
                (x - map.width / 2.0f) * TILE_SIZE,
                0,
                y * TILE_SIZE
            };

            if (isInterior(x, y, map)) {

                glm::vec3 floorPos = {
                    pos.x - FLOOR_OFFSET,
                    -0.1f,
                    pos.z - FLOOR_OFFSET
                };

                SpawnSystem::CreateEntity(
                    world,
                    AssetType::FloorTileLarge,
                    floorPos
                );
            }

            switch (cell) {

                case CellType::Wall:
                    AssetType type;
                    glm::vec3 rot;

                    getWallTypeAndRotation(map, x, y, type, rot);

                    SpawnSystem::CreateEntity(world, type, pos, rot);
                    break;

                case CellType::Door:
                    AssetType wallType;
                    glm::vec3 wallRot;

                    getWallTypeAndRotation(map, x, y, wallType, wallRot);

                    // Scaffold uses same orientation
                    SpawnSystem::CreateEntity(world, AssetType::WallDoorwayScaffold, pos, wallRot);

                    // Door offset based on rotation
                    glm::vec3 offset = {0,0,0};

                    if (rot.y == 0) offset = {-0.82f, 0, 0};
                    else if (rot.y == 90) offset = {0, 0, -0.82f};
                    else if (rot.y == 180) offset = {0.82f, 0, 0};
                    else if (rot.y == -90) offset = {0, 0, 0.82f};

                    auto & door = SpawnSystem::CreateEntity(world, AssetType::Door, pos + offset, wallRot);
                    auto& doorComponent = door.addComponent<Door>("DoorA");
                    doorComponent.baseRotation = wallRot.y;
                    doorComponent.closeAngle = 0.0f;
                    doorComponent.openAngle = 90.0f;
                    doorComponent.currentAngle = 0.0f;
                    doorComponent.targetAngle = 0.0f;
                    break;
            }
        }
    }
}

void BuildRoomSystem::Save(const MapGrid& map, const std::string& file) {

    std::ofstream out(file);

    out << map.width << " " << map.height << "\n";

    for (auto cell : map.cells) {
        out << (int)cell << " ";
    }

    std::cout << "Map saved: " << file << std::endl;
}

MapGrid BuildRoomSystem::Load(const std::string& file) {

    std::ifstream in(file);

    MapGrid map;

    if (!in.is_open()) {
        std::cerr << "Failed to load map: " << file << std::endl;
        return map;
    }

    in >> map.width >> map.height;

    map.cells.resize(map.width * map.height);

    for (int i = 0; i < map.cells.size(); i++) {
        int val;
        in >> val;
        map.cells[i] = (CellType)val;
    }

    std::cout << "Map loaded: " << file << std::endl;

    return map;
}

bool BuildRoomSystem::isWall(const MapGrid& map, int x, int y) {
    if (x < 0 || y < 0 || x >= map.width || y >= map.height)
        return false;

    CellType c = map.cells[y * map.width + x];
    return (c == CellType::Wall || c == CellType::Door);
}

void BuildRoomSystem::getWallTypeAndRotation(const MapGrid& map, int x, int y,
                           AssetType& type, glm::vec3& rotation) {

    bool N = isWall(map, x, y - 1);
    bool S = isWall(map, x, y + 1);
    bool W = isWall(map, x - 1, y);
    bool E = isWall(map, x + 1, y);

    int count = N + S + W + E;

    rotation = {0,0,0};

    // 4 connections → crossing
    if (count == 4) {
        type = AssetType::WallCrossing;
    }

    // 3 connections → T split
    else if (count == 3) {
        type = AssetType::WallTsplit;

        if (!N) rotation = {0,180,0};
        else if (!S) rotation = {0,0,0};
        else if (!W) rotation = {0,90,0};
        else if (!E) rotation = {0,-90,0};
    }

    // 2 connections
    else if (count == 2) {

        // straight
        if ((N && S)) {
            type = AssetType::Wall;
            rotation = {0,90,0};
        }
        else if ((E && W)) {
            type = AssetType::Wall;
            rotation = {0,0,0};
        }

        // corner
        else {
            type = AssetType::WallCorner;

            if (N && E) rotation = {0,180,0};
            else if (E && S) rotation = {0,90,0};
            else if (S && W) rotation = {0,0,0};
            else if (W && N) rotation = {0,-90,0};
        }
    }

    // 1 connection → end cap (use straight wall)
    else {
        type = AssetType::Wall;

        if (N || S) rotation = {0,90,0};
        else rotation = {0,0,0};
    }
}

bool BuildRoomSystem::isInterior(int x, int y, const MapGrid& map) {
    return (x > 0 && x <= map.width - 1 &&
            y > 0 && y <= map.height - 1);
}