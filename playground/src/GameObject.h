//
// Created by adamd on 2026-01-14.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEOBJECT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEOBJECT_H

#include "Game.h"

class GameObject {
public:
    GameObject(const char* path, float x, float y);
    ~GameObject();

    void update(float deltaTime);
    void draw();
private:
    float xPos{}, yPos{};

    SDL_Texture* texture = nullptr;
    SDL_FRect srcRect{}, dstRect{};
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEOBJECT_H