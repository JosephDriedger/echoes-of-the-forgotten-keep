// Created by Joey Driedger

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