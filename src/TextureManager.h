//
// Created by adamd on 2026-01-14.
//

#ifndef ASSIGNMENT1_TEXTUREMANAGER_H
#define ASSIGNMENT1_TEXTUREMANAGER_H

#include <string>
#include <unordered_map>

#include "glad/glad.h"
#include "SDL3_image/SDL_image.h"

class TextureManager {
    static std::unordered_map<std::string, SDL_Texture*> textures;
    static std::unordered_map<std::string, GLuint> textures3D;
public:
    static SDL_Texture* load(const char* path);
    static GLuint load3D(const char* path);

    // floating point Rect = FRect
    static void draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst);
    static void clean();
};

#endif //ASSIGNMENT1_TEXTUREMANAGER_H