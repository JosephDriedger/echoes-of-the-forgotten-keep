//
// Created by adamd on 2026-03-28.
//

#include "engine/scene/BuildRoomSystem.h"

#include <fstream>
#include <random>
#include <iostream>

namespace engine {
    std::vector<SpawnInstance> BuildRoomSystem::Build(const MapGrid& map)
    {
        std::vector<SpawnInstance> instances;

        for (int y = 0; y < map.height; y++) {
            for (int x = 0; x < map.width; x++) {

                glm::vec3 pos = {
                    (x - map.width / 2.0f) * TILE_SIZE,
                    0,
                    y * TILE_SIZE
                };

                // FLOOR
                if (isInterior(x, y, map)) {
                    instances.push_back({
                        engine::PrefabType::FloorLarge,
                        {pos.x - FLOOR_OFFSET, -0.1f, pos.z - FLOOR_OFFSET},
                        0.0f
                    });
                }

                CellType cell = map.cells[y * map.width + x];

                PrefabType type;
                float rotation_y;
                getWallTypeAndRotation(map, x, y, type, rotation_y);

                // WALL
                if (cell == CellType::Wall) {

                    instances.push_back({
                        ConvertToPrefab(type),
                        pos,
                        rotation_y
                    });
                }

                // DOOR
                else if (cell == CellType::Door) {

                    // ✅ Always use scaffold instead of wall
                    instances.push_back({
                        engine::PrefabType::WallDoorwayScaffold,
                        pos,
                        rotation_y
                    });

                    // Door offset
                    glm::vec3 offset{0,0,0};

                    if (rotation_y == 0) offset = {-0.82f, 0, 0};
                    else if (rotation_y == 90) offset = {0, 0, -0.82f};
                    else if (rotation_y == 180) offset = {0.82f, 0, 0};
                    else if (rotation_y == -90) offset = {0, 0, 0.82f};

                    instances.push_back({
                        engine::PrefabType::Door,
                        pos + offset,
                        rotation_y
                    });
                }
            }
        }

        return instances;
    }

    void BuildRoomSystem::getWallTypeAndRotation(
        const MapGrid& map,
        int x, int y,
        PrefabType& type,
        float& rotY)
    {
        bool N = isWall(map, x, y - 1);
        bool S = isWall(map, x, y + 1);
        bool W = isWall(map, x - 1, y);
        bool E = isWall(map, x + 1, y);

        int count = N + S + W + E;

        rotY = 0.0f;

        if (count == 4) {
            type = PrefabType::WallCrossing;
        }
        else if (count == 3) {
            type = PrefabType::WallTsplit;

            if (!N) rotY = 180;
            else if (!S) rotY = 0;
            else if (!W) rotY = 90;
            else if (!E) rotY = -90;
        }
        else if (count == 2) {

            if (N && S) {
                type = PrefabType::Wall;
                rotY = 90;
            }
            else if (E && W) {
                type = PrefabType::Wall;
                rotY = 0;
            }
            else {
                type = PrefabType::WallCorner;

                if (N && E) rotY = 180;
                else if (E && S) rotY = 90;
                else if (S && W) rotY = 0;
                else if (W && N) rotY = -90;
            }
        }
        else {
            type = PrefabType::Wall;

            if (N || S) rotY = 90;
            else rotY = 0;
        }
    }

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

    bool BuildRoomSystem::isInterior(int x, int y, const MapGrid& map) {
        return (x > 0 && x <= map.width - 1 &&
                y > 0 && y <= map.height - 1);
    }


    PrefabType BuildRoomSystem::ConvertToPrefab(PrefabType type)
    {
        switch (type)
        {
            case PrefabType::Wall: return PrefabType::Wall;
            case PrefabType::WallCorner: return PrefabType::WallCorner;
            case PrefabType::WallCrossing: return PrefabType::WallCrossing;
            case PrefabType::WallTsplit: return PrefabType::WallTsplit;
            case PrefabType::WallDoorwayScaffold: return PrefabType::WallDoorwayScaffold;
            default: return PrefabType::Wall;
        }
    }
}