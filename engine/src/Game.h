//
// Created by adamd on 2026-01-07.
//

// #ifndef ASSIGNMENT1_GAME_H
// #define ASSIGNMENT1_GAME_H
//
// #endif //ASSIGNMENT1_GAME_H

#pragma once
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include "World.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Shader.h"

class Game {
public:
    Game();
    ~Game();

    // these are the types that SDL uses
    void init(const char* title, int width, int height, bool fullscreen);

    // game loop functions (handleEvents, update, render)
    void handleEvents(); // check for input and system events
    void update(float deltaTime); // handles the game logic and change to game state
    void render(Uint64 ticks); // handles the drawing of the current game state to the screen

    // used to free resources
    void destroy();

    bool running() {
        return isRunning;
    }

    SDL_Renderer* renderer = nullptr;

    SDL_GLContext glContext = nullptr;

    SceneManager sceneManager;
    static std::function<void (std::string)> onSceneChangeRequest;

private:
    int frameCount = 0;
    bool isRunning = false;

    SDL_Window* window = nullptr;
    SDL_Event event;

    float timeAccumulator;
};
