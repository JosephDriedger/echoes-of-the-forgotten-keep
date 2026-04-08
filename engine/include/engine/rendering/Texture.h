// Created by Adam Van Woerden

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTURE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTURE_H

#include <string>

namespace engine
{
    // Wraps an OpenGL 2D texture loaded from an image file via SDL_image.
    // Converts images to RGBA32 and generates mipmaps on upload.
    // Non-copyable; ownership transfers via move semantics.
    class Texture
    {
    public:
        Texture();
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        bool LoadFromFile(const std::string& path);
        void Destroy();

        // Binds this texture to the given texture unit (GL_TEXTURE0 + slot).
        void Bind(unsigned int slot = 0) const;
        void Unbind() const;

        [[nodiscard]] unsigned int GetId() const;
        [[nodiscard]] int GetWidth() const;
        [[nodiscard]] int GetHeight() const;
        [[nodiscard]] bool IsLoaded() const;

    private:
        unsigned int m_Id;
        int m_Width;
        int m_Height;
        bool m_IsLoaded;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTURE_H