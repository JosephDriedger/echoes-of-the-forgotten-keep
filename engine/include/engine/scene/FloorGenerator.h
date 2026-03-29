//
// Created by adamd on 2026-03-28.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_FLOORGENERATOR_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_FLOORGENERATOR_H

#include <vector>
#include <random>
#include <glm/glm.hpp>

namespace engine {
    enum class CellType {
        Empty,
        Floor,
        Wall,
        Door,
        Start,
        End
    };

    // Final layout grid (logic only)
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

    // Room definition
    struct Room {
        int x, y;
        int width, height;

        glm::ivec2 center() const {
            return { x + width / 2, y + height / 2 };
        }
    };


    struct BuildRoomConfig {
        float tileSize = 4.0f;
        float floorOffset = 2.0f;
        float floorY = -0.1f;
        float doorOffset = 0.82f;
    };

    // Config for generation
    struct FloorConfig {
        int width = 25;
        int height = 25;

        int roomCount = 8;
        int minRoomSize = 4;
        int maxRoomSize = 5;

        float corridorChance = 0.2f;
        float mazeFactor = 0.3f;

        unsigned int seed = 12345;

        BuildRoomConfig buildConfig;
    };

    class FloorGenerator {
    public:
        static FloorLayout Generate(FloorConfig& config);

    private:
        static bool Overlaps(const Room& a, const Room& b);
        static void CarveRoom(FloorLayout& layout, const Room& room);
        static void CarveCorridor(FloorLayout& layout,
                                 const glm::ivec2& a,
                                 const glm::ivec2& b,
                                 std::mt19937& rng);

        static int RandRange(std::mt19937& rng, int min, int max);
        static bool Chance(std::mt19937& rng, float probability);
    };

}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_FLOORGENERATOR_H