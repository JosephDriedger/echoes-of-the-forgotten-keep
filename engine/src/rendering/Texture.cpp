//
// Created by Joseph Driedger on 3/8/2026.
//

#include "engine/rendering/Texture.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glad/gl.h>

#include <iostream>
#include <utility>

namespace engine
{
    Texture::Texture()
        : m_Id(0),
          m_Width(0),
          m_Height(0),
          m_IsLoaded(false)
    {
    }

    Texture::~Texture()
    {
        Destroy();
    }

    Texture::Texture(Texture&& other) noexcept
        : m_Id(other.m_Id),
          m_Width(other.m_Width),
          m_Height(other.m_Height),
          m_IsLoaded(other.m_IsLoaded)
    {
        other.m_Id = 0;
        other.m_Width = 0;
        other.m_Height = 0;
        other.m_IsLoaded = false;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        Destroy();

        m_Id = other.m_Id;
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_IsLoaded = other.m_IsLoaded;

        other.m_Id = 0;
        other.m_Width = 0;
        other.m_Height = 0;
        other.m_IsLoaded = false;

        return *this;
    }

    bool Texture::LoadFromFile(const std::string& path)
    {
        Destroy();

        SDL_Surface* loadedSurface = IMG_Load(path.c_str());

        if (loadedSurface == nullptr)
        {
            std::cerr << "[Texture] Failed to load image: "
                      << path
                      << " ("
                      << SDL_GetError()
                      << ")\n";
            return false;
        }

        SDL_Surface* convertedSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(loadedSurface);
        loadedSurface = nullptr;

        if (convertedSurface == nullptr)
        {
            std::cerr << "[Texture] Failed to convert image to RGBA32: "
                      << path
                      << " ("
                      << SDL_GetError()
                      << ")\n";
            return false;
        }

        m_Width = convertedSurface->w;
        m_Height = convertedSurface->h;

        glGenTextures(1, &m_Id);
        glBindTexture(GL_TEXTURE_2D, m_Id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            m_Width,
            m_Height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            convertedSurface->pixels
        );

        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        SDL_DestroySurface(convertedSurface);

        m_IsLoaded = true;
        return true;
    }

    void Texture::Destroy()
    {
        if (m_Id != 0)
        {
            glDeleteTextures(1, &m_Id);
            m_Id = 0;
        }

        m_Width = 0;
        m_Height = 0;
        m_IsLoaded = false;
    }

    void Texture::Bind(const unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_Id);
    }

    void Texture::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    unsigned int Texture::GetId() const
    {
        return m_Id;
    }

    int Texture::GetWidth() const
    {
        return m_Width;
    }

    int Texture::GetHeight() const
    {
        return m_Height;
    }

    bool Texture::IsLoaded() const
    {
        return m_IsLoaded;
    }
}