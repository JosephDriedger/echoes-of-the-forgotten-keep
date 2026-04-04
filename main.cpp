// Created by Adam Van Woerden

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