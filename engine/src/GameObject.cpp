//
// Created by adamd on 2026-01-14.
//

#include "GameObject.h"

#include "TextureManager.h"

GameObject::GameObject(const char *path, float x, float y) :
xPos(x), yPos(y), texture(TextureManager::load(path))
{}

GameObject::~GameObject() {
    if (texture) SDL_DestroyTexture(texture);
}

void GameObject::update(float deltaTime) {

    //move gameObject one pixel on the x and y each frame
    xPos += 60 * deltaTime;
    yPos += 60 * deltaTime;

    // where do we want to crop the texture from?
    srcRect.x = srcRect.y = 0;

    // how much to crop?
    srcRect.w = srcRect.h = 32;

    // where to draw it?
    // static_cast<float>
    dstRect.x = xPos;
    dstRect.y = yPos;

    // what sizes to render?
    dstRect.h = srcRect.h;
    dstRect.w = srcRect.w;
}

void GameObject::draw() {
    TextureManager::draw(texture, srcRect, dstRect);
}