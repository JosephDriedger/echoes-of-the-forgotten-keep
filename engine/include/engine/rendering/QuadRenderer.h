#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_QUADRENDERER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_QUADRENDERER_H

#include <memory>
#include <string>

namespace engine
{
    class Shader;

    class QuadRenderer
    {
    public:
        QuadRenderer();
        ~QuadRenderer();

        QuadRenderer(const QuadRenderer&) = delete;
        QuadRenderer& operator=(const QuadRenderer&) = delete;

        bool Initialize(const std::string& vertShaderPath,
                        const std::string& fragShaderPath,
                        int screenWidth, int screenHeight);
        void Destroy();

        void DrawFilledRect(float x, float y, float width, float height,
                            float r, float g, float b, float a = 1.0f);

    private:
        std::shared_ptr<Shader> m_Shader;
        unsigned int m_VAO;
        unsigned int m_VBO;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_QUADRENDERER_H
