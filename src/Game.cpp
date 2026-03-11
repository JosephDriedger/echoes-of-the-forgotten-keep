//
// Created by adamd on 2026-01-07.
//

#include "Game.h"
#include "Map.h"
#include <iostream>
#include <ostream>
#include "manager/AssetManager.h"
#include <assimp/postprocess.h>

int updateColor = 1000;

std::function<void(std::string)> Game::onSceneChangeRequest;

Game::Game() {}

Game::~Game() {
    destroy();
}

void Game::init(const char *title, int width, int height, bool fullscreen) {

    int flags = 0;
    if (fullscreen) {

        flags = SDL_WINDOW_FULLSCREEN;
    }

    // initialize SDL library
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 1) {
        std::cout << "Subsystem initialized..." << std::endl;

        // Tell SDL we want OpenGL 3.3 Core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


        window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
        if (window) {
            std::cout << "Window created..." << std::endl;
        }

        // windows will be direct3D (directx)
        // Mac is will likely be metal, OpenGL
        renderer = SDL_CreateRenderer(window, nullptr); // KEEP for UI?

        if (renderer) {
            std::cout << "renderer created..." << std::endl;
        }
        else {
            std::cout << "renderer could not be created..." << std::endl;
        }

        // Create OpenGL context
        glContext = SDL_GL_CreateContext(window);

        if (glContext) {
            std::cout << "OpenGL context created..." << std::endl;
        } else {
            std::cout << "Failed to create OpenGL context..." << std::endl;
        }

        SDL_GL_MakeCurrent(window, glContext);
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            std::cout << "Failed to initialize GLAD!" << std::endl;
        }

        isRunning = true;
    }
    else {
        isRunning = false;
    }

    // load assets
    AssetManager::loadAnimation("player", "../asset/animations/Swordsman_animations.xml");
    AssetManager::loadAnimation("enemy", "../asset/animations/bird_animations.xml");

    //load scenes
    sceneManager.loadScene("level1", "../asset/map.tmx", width, height);
    sceneManager.loadScene("level2", "../asset/map2.tmx", width, height);

    // start level 1
    sceneManager.changeSceneDeferred("level1");

    // resolve scene callback
    onSceneChangeRequest = [this](std::string sceneName) {

        if (sceneManager.currentScene->getName() == "level2" && sceneName == "level2") {
            std::cout << "You win!" << std::endl;
            isRunning = false;
            return;
        }

        if (sceneName == "gameover") {
            std::cout << "Game Over!" << std::endl;

            isRunning = false;
            return;
        }

        sceneManager.changeSceneDeferred(sceneName);
    };

    glEnable(GL_DEPTH_TEST);

}

void Game::handleEvents() {
    // SDL listens to the OS for input events internally and it ands them to a queue

    // check for event, if an event is available it will remove from the queue and store in event
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_EVENT_QUIT: // usually triggered when the user closes the window
        isRunning = false;
            break;
        default:
            break;
    }
}

void Game::update(float dt) {
    sceneManager.update(dt, event);
}

void Game::render(Uint64 ticks)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sceneManager.render();

    SDL_GL_SwapWindow(window);

}

void Game::destroy() {
    TextureManager::clean();
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    // Delete OpenGL context
    if (glContext)
    {
        SDL_GL_DestroyContext(glContext);
        glContext = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
    std::cout << "Game Destroyed..." << std::endl;
}
