//
// Created by adamd on 2026-01-14.
//

#include "TextureManager.h"
#include "../Game.h"
#include <iostream>
#include <ostream>

// doesn't create any extra memory, just grabs the reference of the object
extern Game* game;
std::unordered_map<std::string, SDL_Texture*> TextureManager::textures;
std::unordered_map<std::string, GLuint*> TextureManager::textures3D;

// load our texture with a path
SDL_Texture* TextureManager::load(const char* path) {

    // Cashing: storing the result of some work so you don't have to repeat the work next time
    // Check if the texture already exists in the map
    auto it = textures.find(path);
    if (it != textures.end()) { // if the iterator is not at the end
        return it->second;
    }

    // what is a surface?
    // a surface is an image in ram (cpu memory)
    // do we want to do anything with the image before we store it in gpu?
    SDL_Surface* tempSurface = IMG_Load(path);
    if(!tempSurface) {
        std::cout << "Failed to load image " << path << std::endl;
    }

    // texture represents an image in ram (gpu memory)
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, tempSurface);

    // cleanup surface
    SDL_DestroySurface(tempSurface);

    // Check if the texture creation was successful
    if (!texture) {
        std::cout << "Failed to create texture from " << path << std::endl;
        return nullptr;
    }

    // store the new texture in the cache
    textures[path] = texture;

    return texture;
}

GLuint* TextureManager::load3D(const char* path) {
    // check cache
    auto it = textures3D.find(path);
    if (it != textures3D.end()) return it->second;

    SDL_Surface* surface = IMG_Load(path);
    if(!surface) {
        std::cout << "Failed to load image " << path << std::endl;
        return 0;
    }

    GLuint* id;
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_DestroySurface(surface);

    textures3D[path] = id;
    return id;
}

void TextureManager::draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst) {

    // draw the texture to the screen from the vram
    SDL_RenderTexture(game->renderer, texture, &src, &dst);
}

void TextureManager::clean() {
    for (auto& tex: textures) {
        SDL_DestroyTexture(tex.second);
        tex.second = nullptr;
    }
    textures.clear();
    for (auto& tex : textures3D) {
        glDeleteTextures(1, tex.second);
    }
    textures3D.clear();
}
