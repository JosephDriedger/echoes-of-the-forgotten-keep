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
#include <SDL3/SDL_opengl.h>

#include "World.h"
#include "manager/Scene.h"
#include "manager/SceneManager.h"
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

    void week4output();
    void week1(Uint64 ticks);

    // used to free resources
    void destroy();

    bool running() {
        return isRunning;
    }

    SDL_Renderer* renderer = nullptr;

    SDL_GLContext glContext = nullptr;

    SceneManager sceneManager;
    //SceneManager* sceneManager;
    static std::function<void (std::string)> onSceneChangeRequest;

private:
    int frameCount = 0;
    bool isRunning = false;

    SDL_Window* window = nullptr;
    SDL_Event event;

    // SDL stores colors channels as 8-bit unsigned
    // range is 0-255
    Uint8 r=255;
    Uint8 g=255;
    Uint8 b=255;
    Uint8 a=255;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint shaderProgram = 0;

    GLuint cubeVAO = 0, cubeVBO = 0;
    GLuint cubeTexture = 0;

    Shader* shader;

    std::vector<Mesh> meshes;

    float colorTimer = 0.0f;

};
