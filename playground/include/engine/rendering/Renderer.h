//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERER_H

namespace engine
{
    class Window;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        bool Initialize(Window& window);
        void Shutdown();

        void BeginFrame();
        void EndFrame();

        bool IsInitialized() const;

    private:
        Window* m_Window;
        bool m_IsInitialized;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERER_H