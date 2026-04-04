// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERER_H

namespace engine
{
    class Window;
    class Camera;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        bool Initialize(Window& window);
        void Shutdown();

        void BeginFrame();
        void EndFrame();

        void SetActiveCamera(const Camera* camera);

        [[nodiscard]] const Camera* GetActiveCamera() const;
        [[nodiscard]] bool IsInitialized() const;

    private:
        Window* m_Window;
        const Camera* m_ActiveCamera;
        bool m_IsInitialized;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERER_H