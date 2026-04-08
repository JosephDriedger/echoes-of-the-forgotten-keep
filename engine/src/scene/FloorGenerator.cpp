// Created by Joey Driedger
//
// Procedural dungeon generation using random room placement and L-shaped corridor carving.
// The algorithm is seeded for deterministic replay: the same seed always produces the same layout.

#include "engine/scene/FloorGenerator.h"

#include <algorithm>
#include <iostream>

namespace engine {

// ---------------- RNG helpers ----------------

int FloorGenerator::RandRange(std::mt19937& rng, int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

bool FloorGenerator::Chance(std::mt19937& rng, float probability) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng) < probability;
}

// ---------------- Room helpers ----------------

bool FloorGenerator::Overlaps(const Room& a, const Room& b) {
    return (a.x < b.x + b.width &&
            a.x + a.width > b.x &&
            a.y < b.y + b.height &&
            a.y + a.height > b.y);
}

void FloorGenerator::CarveRoom(FloorLayout& layout, const Room& room) {
    for (int y = room.y; y < room.y + room.height; y++) {
        for (int x = room.x; x < room.x + room.width; x++) {
            layout.get(x, y) = CellType::Floor;
        }
    }
}

// ---------------- Corridor ----------------
// Carves an L-shaped (two-segment) corridor between points a and b.
// The bend direction is randomized: horizontal-then-vertical or vertical-then-horizontal.
// This creates more organic-looking paths than straight corridors.

void FloorGenerator::CarveCorridor(FloorLayout& layout,
                                   const glm::ivec2& a,
                                   const glm::ivec2& b,
                                   std::mt19937& rng)
{
    if (Chance(rng, 0.5f)) {
        // Horizontal first, then vertical at b.x
        for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); x++)
            layout.get(x, a.y) = CellType::Floor;

        for (int y = std::min(a.y, b.y); y <= std::max(a.y, b.y); y++)
            layout.get(b.x, y) = CellType::Floor;
    } else {
        // Vertical first, then horizontal at b.y
        for (int y = std::min(a.y, b.y); y <= std::max(a.y, b.y); y++)
            layout.get(a.x, y) = CellType::Floor;

        for (int x = std::min(a.x, b.x); x <= std::max(a.x, b.x); x++)
            layout.get(x, b.y) = CellType::Floor;
    }
}

// ---------------- Main Generate ----------------

FloorLayout FloorGenerator::Generate(FloorConfig& config) {

    std::mt19937 rng(config.seed);

    FloorLayout layout;
    layout.width = config.width;
    layout.height = config.height;
    layout.cells.resize(config.width * config.height, CellType::Empty);

    std::vector<Room> rooms;

    // ---- Phase 1: Place rooms ----
    // Randomly size and position rooms within the grid (with a 1-cell border).
    // Rooms that overlap an existing room are discarded, so the final count
    // may be less than config.roomCount.
    for (int i = 0; i < config.roomCount; i++) {

        int w = RandRange(rng, config.minRoomSize, config.maxRoomSize);
        int h = RandRange(rng, config.minRoomSize, config.maxRoomSize);

        int x = RandRange(rng, 1, config.width - w - 2);
        int y = RandRange(rng, 1, config.height - h - 2);

        Room newRoom {x, y, w, h};

        bool overlaps = false;
        for (const auto& r : rooms) {
            if (Overlaps(newRoom, r)) {
                overlaps = true;
                break;
            }
        }

        if (!overlaps) {
            CarveRoom(layout, newRoom);
            rooms.push_back(newRoom);
        }
    }

    if (rooms.empty()) {
        std::cerr << "[FloorGenerator] No rooms generated\n";
        return layout;
    }

    // ---- Phase 2: Connect consecutive rooms ----
    // Guarantees every room is reachable by linking room[i-1] to room[i].
    for (size_t i = 1; i < rooms.size(); i++) {

        glm::ivec2 a = rooms[i - 1].center();
        glm::ivec2 b = rooms[i].center();

        CarveCorridor(layout, a, b, rng);
    }

    // ---- Phase 3: Add extra corridors for loop complexity ----
    // mazeFactor controls how many additional random connections are made,
    // creating alternate paths and making the dungeon feel less linear.
    int extra = static_cast<int>(config.mazeFactor * rooms.size());

    for (int i = 0; i < extra; i++) {
        int a = RandRange(rng, 0, rooms.size() - 1);
        int b = RandRange(rng, 0, rooms.size() - 1);

        if (a != b) {
            CarveCorridor(layout,
                          rooms[a].center(),
                          rooms[b].center(),
                          rng);
        }
    }

    // ---- Start / End ----
    layout.get(rooms.front().center().x,
               rooms.front().center().y) = CellType::Start;

    layout.get(rooms.back().center().x,
               rooms.back().center().y) = CellType::End;

    return layout;
}

}