// Created by Adam Van Woerden

// Entry point for Echoes of the Forgotten Keep.
// Creates the game application and runs the main loop. All unhandled
// exceptions are caught here so they can be logged before exit.

#define SDL_MAIN_HANDLED

#include "game/GameApp.h"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        game::GameApp app;
        return app.Run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[EFK] Unhandled exception: " << exception.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "[EFK] Unhandled unknown exception.\n";
    }

    return -1;
}