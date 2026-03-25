//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATIONSPECIFICATION_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATIONSPECIFICATION_H

#include <string>

namespace engine
{
    struct ApplicationSpecification
    {
        std::string Title = "EFK Application";

        int Width = 1280;
        int Height = 720;

        double TargetFrameTimeMs = 1000.0 / 60.0; // ~60 FPS
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATIONSPECIFICATION_H