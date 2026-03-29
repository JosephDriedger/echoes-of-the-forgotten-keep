//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTURE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_TEXTURE_H

#include <string>

namespace engine
{
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