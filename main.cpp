#include "engine/core/Application.h"

#include <iostream>

int main()
{
    engine::Application app;

    if (!app.Initialize())
    {
        std::cerr << "Engine failed to initialize\n";
        return -1;
    }

    std::cout << "Engine initialized successfully\n";

    app.Shutdown();

    return 0;
}
