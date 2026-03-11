//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H

#pragma once

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

    private:
        std::unique_ptr<Window> m_Window;
    };
}

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_APPLICATION_H