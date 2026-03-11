//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H

#include "engine/core/Time.h"

#include <memory>

namespace engine
{
    class Window;

    class Application
    {
    public:
        Application();
        ~Application();

        bool Initialize();
        void Shutdown();

        Timestep Tick();

    private:
        std::unique_ptr<Window> m_Window;
        Time m_Time;
        bool m_IsInitialized;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H