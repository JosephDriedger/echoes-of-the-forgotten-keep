#include <iostream>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>


#include "Game.h"


// global variable
Game *game = nullptr;


// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {

    const int FPS = 60; // 60 is the closest refresh rate of most our monitors, 30fps is th half the work
    const int desiredFrameTime = 1000 / FPS; // 16ms per frame

    Uint64 ticks = SDL_GetTicks();
    int actualFrameTime = 0;
    float deltaTime = 0.0f;

    game = new Game();
    game->init("echoes_of_the_forgotten_keep Engine", 800, 600, false);

    // game loop
    while (game->running()) {

         // time in milliseconds since we initialized SDL
        int currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - ticks) / 1000.0f;
        ticks = currentTicks;
        game->handleEvents();
        game->update(deltaTime);
        game->render(ticks);

        actualFrameTime = SDL_GetTicks() - ticks; // elapsed time in milliseconds it took the current frame

        // frame limiter
        // keeps the game running at the desired frame rate
        // if the actual frame took less time then the desired frame, delay the difference
        if (desiredFrameTime > actualFrameTime) {
            SDL_Delay(desiredFrameTime - actualFrameTime);
        }
    }

    delete game;

    return 0;
}