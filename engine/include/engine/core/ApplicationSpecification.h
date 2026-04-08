// Created by Joey Driedger

// Configuration data supplied when constructing an Application.
// Provides window title, dimensions, and target frame rate.

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

        // Target frame duration in milliseconds (used for frame pacing).
        double TargetFrameTimeMs = 1000.0 / 60.0; // ~60 FPS
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATIONSPECIFICATION_H