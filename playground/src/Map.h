//
// Created by adamd on 2026-01-14.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MAP_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MAP_H

#include <vector>
#include <SDL3/SDL.h>
#include <Component.h>

class Map {
public:
    Map() = default;
    ~Map() = default;

    void load(const char* path, SDL_Texture *ts);
    void draw(const Camera &cam);

    SDL_Texture* tileset = nullptr;
    int width {}, height {}, tileWidth {}, tileHeight {};
    std::vector<std::vector<int>> tileData;
    std::vector<Collider> colliders;
    std::vector<Collider> items;
    std::vector<Collider> spawner;

};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_MAP_H