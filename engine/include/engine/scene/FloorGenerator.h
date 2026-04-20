// Created by Joey Driedger
//
// Procedural dungeon floor generator.
// Uses a room-placement + corridor-carving algorithm to produce a 2D FloorLayout
// that BuildRoomSystem later converts into 3D geometry.

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_FLOORGENERATOR_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_FLOORGENERATOR_H

#include <vector>
#include <random>
#include <glm/glm.hpp>

namespace engine {

    // Tile types used throughout the dungeon generation pipeline.
    enum class CellType {
        Empty,  // Void / unused space
        Floor,  // Walkable surface
        Wall,   // Solid boundary (added by BuildRoomSystem::FromFloor)
        Door,   // Doorway between corridor and room
        Start,  // Player spawn / floor entry point
        End     // Stairs down to the next floor
    };

    // Row-major 2D grid of CellType values representing a single dungeon floor.
    // Contains only logical tile data; BuildRoomSystem handles 3D conversion.
    struct FloorLayout {
        int width = 0;
        int height = 0;
        std::vector<CellType> cells;

        CellType& get(int x, int y) {
            return cells[y * width + x];
        }

        const CellType& get(int x, int y) const {
            return cells[y * width + x];
        }
    };

    // Axis-aligned rectangle representing a single room in the dungeon.
    struct Room {
        int x, y;
        int width, height;

        glm::ivec2 center() const {
            return { x + width / 2, y + height / 2 };
        }
    };

    // Spatial configuration for converting MapGrid cells to world-space positions.
    struct BuildRoomConfig {
        float tileSize = 4.0f;      // World units per grid cell
        float floorOffset = 2.0f;   // Half-tile offset for centering large floor tiles
        float floorY = -0.1f;       // Vertical position of floor geometry
        float doorOffset = 0.82f;   // How far the door prop is offset from the wall center
    };

    // Parameters controlling procedural floor generation.
    struct FloorConfig {
        int width = 40;
        int height = 40;

        int roomCount = 8;
        int minRoomSize = 4;
        int maxRoomSize = 5;

        // Width (in cells) of every carved corridor. Keep at 1 so the door
        // detector (which looks for corridor cells with exactly 2 walkable
        // neighbors) can find room/corridor transitions to place doors at.
        int corridorWidth = 1;

        float corridorChance = 0.2f;
        // Fraction of extra corridors added between random room pairs to create loops.
        float mazeFactor = 0.3f;

        unsigned int seed = 12345;

        BuildRoomConfig buildConfig;
    };

    // Generates a FloorLayout by:
    //   1. Placing non-overlapping rooms at random positions
    //   2. Connecting consecutive rooms with L-shaped corridors
    //   3. Adding extra corridors (mazeFactor) for loop complexity
    //   4. Marking the first room's center as Start and the last as End
    class FloorGenerator {
    public:
        static FloorLayout Generate(FloorConfig& config);

    private:
        // Single generation attempt. Generate() calls this in a retry loop
        // and rejects results that would produce wall peninsulas/islands.
        static FloorLayout GenerateOnce(FloorConfig& config);
        static bool Overlaps(const Room& a, const Room& b);
        static void CarveRoom(FloorLayout& layout, const Room& room);
        // Carves an L-shaped corridor between two points.
        // Randomly chooses horizontal-first or vertical-first order.
        static void CarveCorridor(FloorLayout& layout,
                                 const glm::ivec2& a,
                                 const glm::ivec2& b,
                                 std::mt19937& rng,
                                 int width);

        static int RandRange(std::mt19937& rng, int min, int max);
        static bool Chance(std::mt19937& rng, float probability);
    };

}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_FLOORGENERATOR_H