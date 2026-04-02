// Created by Joey Driedger

#include "engine/scene/BuildRoomSystem.h"

#include <fstream>
#include <random>
#include <iostream>

namespace engine {
    std::vector<SpawnInstance> BuildRoomSystem::Build(
    const MapGrid& map,
    const BuildRoomConfig& config)
    {
        std::vector<SpawnInstance> result;

        for (int y = 0; y < map.height; y++) {
            for (int x = 0; x < map.width; x++) {

                CellType cell = map.cells[y * map.width + x];

                glm::vec3 pos = {
                    (x - map.width / 2.0f) * config.tileSize,
                    0,
                    y * config.tileSize
                };
                // check north and west
                bool northEmpty = true;
                bool westEmpty  = true;
                bool northWestEmpty  = true;

                if (y > 0)
                    northEmpty = (map.cells[(y - 1) * map.width + x] == CellType::Empty);

                if (x > 0)
                    westEmpty = (map.cells[y * map.width + (x - 1)] == CellType::Empty);

                if (y > 0 && x > 0)
                    northWestEmpty = (map.cells[(y - 1) * map.width + (x - 1)] == CellType::Empty);

                // FLOOR
                if (cell != CellType::Empty
                    && !northEmpty
                    && !westEmpty
                    && !northWestEmpty
                    && cell != CellType::Start
                    && cell != CellType::End) {

                    result.push_back({
                        PrefabType::FloorLarge,
                        {
                            pos.x - config.floorOffset,
                            config.floorY,
                            pos.z - config.floorOffset
                        },
                        0
                    });
                }

                if (cell == CellType::Start) {
                    result.push_back({
                        PrefabType::Stairs,
                        {
                            pos.x - config.floorOffset,
                            config.floorY,
                            pos.z - config.floorOffset * 2
                        },
                        0
                    });
                }

                if (cell == CellType::End) {
                    result.push_back({
                        PrefabType::Stairs,
                        {
                            pos.x - config.floorOffset,
                            config.floorY - 3.8,
                            pos.z - config.floorOffset * 2
                        },
                        0
                    });
                }

                // WALLS
                if (cell == CellType::Wall || cell == CellType::Door) {

                    PrefabType type;
                    float rotation_y;

                    getWallTypeAndRotation(map, x, y, type, rotation_y);

                    // Door uses doorway instead
                    if (cell == CellType::Door) {
                        type = PrefabType::WallDoorway;
                    }

                    result.push_back({ type, pos, rotation_y });

                    // Spawn actual door
                    if (cell == CellType::Door) {

                        glm::vec3 offset = {0,0,0};

                        if (rotation_y == 0) offset = {-config.doorOffset, 0, 0};
                        else if (rotation_y == 90) offset = {0, 0, config.doorOffset};
                        else if (rotation_y == 180) offset = {config.doorOffset, 0, 0};
                        else if (rotation_y == -90) offset = {0, 0, -config.doorOffset};

                        result.push_back({
                            PrefabType::Door,
                            pos + offset,
                            rotation_y
                        });
                    }
                }
            }
        }

        return result;
    }

    MapGrid BuildRoomSystem::FromFloor(const FloorLayout& floor)
    {
        MapGrid map;
        map.width = floor.width;
        map.height = floor.height;
        map.cells.resize(map.width * map.height, CellType::Empty);

        // Step 1: Copy floor data
        for (int y = 0; y < floor.height; y++) {
            for (int x = 0; x < floor.width; x++) {

                auto cell = floor.get(x, y);

                if (cell == CellType::Floor)
                {
                    map.get(x, y) = CellType::Floor;
                }
                if (cell == CellType::Start)
                {
                    map.get(x, y) = CellType::Start;
                }if (cell == CellType::End)
                {
                    map.get(x, y) = CellType::End;
                }
            }
        }

        // Step 2: Add walls around floors (including diagonals)
        for (int y = 0; y < map.height; y++) {
            for (int x = 0; x < map.width; x++) {

                if (map.get(x, y) != CellType::Floor)
                    continue;

                // 8 directions (N, S, W, E + diagonals)
                const int dx[8] = {0, 0, -1, 1, -1, 1, -1, 1};
                const int dy[8] = {-1, 1, 0, 0, -1, -1, 1, 1};

                for (int i = 0; i < 8; i++) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];

                    if (nx < 0 || ny < 0 || nx >= map.width || ny >= map.height)
                        continue;

                    if (map.get(nx, ny) == CellType::Empty) {
                        map.get(nx, ny) = CellType::Wall;
                    }
                }
            }
        }

        // Step 3: Detect room entrances (doors)
        // Step 3: Detect doors ONLY at room entrances
        for (int y = 1; y < map.height - 1; y++) {
            for (int x = 1; x < map.width - 1; x++) {

                if (map.get(x, y) != CellType::Floor)
                    continue;

                int adj = CountAdjacentFloors(map, x, y);

                // must be corridor
                if (adj != 2)
                    continue;

                bool N = isFloorLike(map.get(x, y - 1));
                bool S = isFloorLike(map.get(x, y + 1));
                bool W = isFloorLike(map.get(x - 1, y));
                bool E = isFloorLike(map.get(x + 1, y));

                // orientation check
                bool vertical   = N && S && !E && !W;
                bool horizontal = E && W && !N && !S;

                if (!(vertical || horizontal))
                    continue;

                // 🔥 NEW: detect if touching a ROOM
                bool nearRoom = false;

                const int dx[4] = {0, 0, -1, 1};
                const int dy[4] = {-1, 1, 0, 0};

                for (int i = 0; i < 4; i++) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];

                    if (CountAdjacentFloors(map, nx, ny) >= 3) {
                        nearRoom = true;
                        break;
                    }
                }

                if (!nearRoom)
                    continue;

                // avoid overwriting special tiles
                if (map.get(x, y) == CellType::Start ||
                    map.get(x, y) == CellType::End)
                    continue;

                map.get(x, y) = CellType::Door;
            }
        }

        return map;
    }

    bool BuildRoomSystem::isFloorLike(CellType c)
    {
        return (c == CellType::Floor ||
                c == CellType::Start ||
                c == CellType::End);
    }

    int BuildRoomSystem::CountAdjacentFloors(MapGrid& map, int x, int y)
    {
        int count = 0;

        const int dx[4] = {0, 0, -1, 1};
        const int dy[4] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (map.get(nx, ny) == CellType::Floor ||
                map.get(nx, ny) == CellType::Start ||
                map.get(nx, ny) == CellType::End)
            {
                count++;
            }
        }

        return count;
    }

    bool BuildRoomSystem::IsRoomTile(MapGrid& map, int x, int y)
    {
        int floorCount = 0;

        const int dx[4] = {0, 0, -1, 1};
        const int dy[4] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (map.get(nx, ny) == CellType::Floor ||
                map.get(nx, ny) == CellType::Start ||
                map.get(nx, ny) == CellType::End)
            {
                floorCount++;
            }
        }

        return floorCount >= 3; // inside room-ish area
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

            if (!N) rotY = 0;
            else if (!S) rotY = 180;
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
            case PrefabType::WallDoorway: return PrefabType::WallDoorway;
            default: return PrefabType::Wall;
        }
    }

    void BuildRoomSystem::DebugPrint(const MapGrid& map)
    {
        std::cout << "\n=== MAP DEBUG ===\n";

        for (int y = 0; y < map.height; y++)
        {
            for (int x = 0; x < map.width; x++)
            {
                CellType c = map.cells[y * map.width + x];

                char symbol = '?';

                switch (c)
                {
                    case CellType::Empty: symbol = ' '; break;
                    case CellType::Floor: symbol = '.'; break;
                    case CellType::Wall:  symbol = '#'; break;
                    case CellType::Door:  symbol = 'D'; break;

                    case CellType::Start: symbol = 'S'; break;
                    case CellType::End:   symbol = 'E'; break;
                }

                std::cout << symbol;
            }
            std::cout << "\n";
        }

        std::cout << "=================\n";
    }
}