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
    // Large padding forces rooms apart so the corridors connecting them
    // are actually long passages, not shared walls. Rooms that can't find
    // a spot with this much clearance just get discarded. Increased so
    // corridors routed between room centers can't graze a third room's wall.
    const int kPad = 8;
    return (a.x - kPad < b.x + b.width &&
            a.x + a.width + kPad > b.x &&
            a.y - kPad < b.y + b.height &&
            a.y + a.height + kPad > b.y);
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
                                   std::mt19937& rng,
                                   int width)
{
    if (width < 1) width = 1;

    auto carveHoriz = [&](int x0, int x1, int y) {
        for (int x = std::min(x0, x1); x <= std::max(x0, x1); x++) {
            for (int w = 0; w < width; w++) {
                int yy = y + w;
                if (yy < 0 || yy >= layout.height) continue;
                if (x < 0 || x >= layout.width) continue;
                if (layout.get(x, yy) != CellType::Start &&
                    layout.get(x, yy) != CellType::End) {
                    layout.get(x, yy) = CellType::Floor;
                }
            }
        }
    };

    auto carveVert = [&](int y0, int y1, int x) {
        for (int y = std::min(y0, y1); y <= std::max(y0, y1); y++) {
            for (int w = 0; w < width; w++) {
                int xx = x + w;
                if (xx < 0 || xx >= layout.width) continue;
                if (y < 0 || y >= layout.height) continue;
                if (layout.get(xx, y) != CellType::Start &&
                    layout.get(xx, y) != CellType::End) {
                    layout.get(xx, y) = CellType::Floor;
                }
            }
        }
    };

    if (Chance(rng, 0.5f)) {
        carveHoriz(a.x, b.x, a.y);
        carveVert(a.y, b.y, b.x);
    } else {
        carveVert(a.y, b.y, a.x);
        carveHoriz(a.x, b.x, b.y);
    }
}

// ---------------- Main Generate ----------------

// Validates a candidate layout. Rejects layouts that would produce thin
// wall peninsulas or enclosed wall islands, which appear as ugly "circular"
// wall structures inside otherwise-open rooms. The conditions flagged:
//   - An Empty cell with Floor on opposite cardinal sides (N&S or E&W) --
//     this becomes a wall strip separating two parallel walkable strips.
//   - An Empty cell surrounded by Floor on 3+ cardinal sides -- this
//     becomes a wall peninsula jutting into a room.
static bool ValidateLayout(const FloorLayout& layout) {
    auto floorLike = [](CellType c) {
        return c == CellType::Floor || c == CellType::Start || c == CellType::End;
    };
    for (int y = 1; y < layout.height - 1; y++) {
        for (int x = 1; x < layout.width - 1; x++) {
            if (layout.get(x, y) != CellType::Empty) continue;
            bool N = floorLike(layout.get(x, y - 1));
            bool S = floorLike(layout.get(x, y + 1));
            bool W = floorLike(layout.get(x - 1, y));
            bool E = floorLike(layout.get(x + 1, y));
            if ((N && S) || (E && W)) return false;
            if ((N + S + W + E) >= 3) return false;
        }
    }
    return true;
}

FloorLayout FloorGenerator::Generate(FloorConfig& config) {
    const int kMaxAttempts = 50;
    const unsigned int baseSeed = config.seed;
    FloorLayout best;

    for (int attempt = 0; attempt < kMaxAttempts; attempt++) {
        config.seed = baseSeed + static_cast<unsigned int>(attempt);
        FloorLayout candidate = GenerateOnce(config);
        if (ValidateLayout(candidate)) {
            if (attempt > 0) {
                std::cerr << "[FloorGenerator] Accepted on attempt "
                          << (attempt + 1) << "\n";
            }
            config.seed = baseSeed;
            return candidate;
        }
        best = std::move(candidate);
    }

    std::cerr << "[FloorGenerator] Exhausted " << kMaxAttempts
              << " attempts without a clean layout; using last.\n";
    config.seed = baseSeed;
    return best;
}

FloorLayout FloorGenerator::GenerateOnce(FloorConfig& config) {

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

        CarveCorridor(layout, a, b, rng, config.corridorWidth);
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
                          rng,
                          config.corridorWidth);
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