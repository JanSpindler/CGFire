//
// Created by JS on 26/05/2021.
//

#include "engine/gr_include.hpp"
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "engine/Render/GLTexture.hpp"
#include "engine/Util.hpp"

namespace en
{
    GLTexture::GLTexture()
    {
        glGenTextures(1, &handle_);
    }

    GLTexture::~GLTexture()
    {
    }

    GLPictureTex::GLPictureTex(WrapMode wrapMode, FilterMode filterMode, const std::string& path, bool flipUv) :
        GLTexture()
    {
        Log::Info(path);

        int width;
        int height;
        int channelCount;
        stbi_set_flip_vertically_on_load(flipUv);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);
        if (!data)
            Log::Error("Failed to load Texture", true);
        Log::Info("Channel count " + std::to_string(channelCount));

        glBindTexture(GL_TEXTURE_2D, handle_);

        GLenum glFormat;
        switch (channelCount)
        {
            case 3:
                glFormat = GL_RGB;
                break;
            case 4:
                glFormat = GL_RGBA;
                break;
            default:
                Log::Error("Invalid channel count " + std::to_string(channelCount), true);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);

        GLenum glWrapMode;
        switch (wrapMode)
        {
            case WrapMode::REPEAT:
                glWrapMode = GL_REPEAT;
                break;
            case WrapMode::MIRRORED_REPEAT:
                glWrapMode = GL_MIRRORED_REPEAT;
                break;
            case WrapMode::CLAMP_TO_EDGE:
                glWrapMode = GL_CLAMP_TO_EDGE;
                break;
            case WrapMode::CLAMP_TO_BORDER:
                glWrapMode = GL_CLAMP_TO_BORDER;
                break;
            default:
                Log::Error("Unknown GLPictureTex::WrapMode", true);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode);

        GLenum glFilterMode;
        switch(filterMode)
        {
            case FilterMode::NEAREST:
                glFilterMode = GL_NEAREST;
                break;
            case FilterMode::LINEAR:
                glFilterMode = GL_LINEAR;
                break;
            default:
                Log::Error("Unknown GLPictureTex::FilterMode", true);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMode);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    GLPictureTex::~GLPictureTex()
    {
    }

    void GLPictureTex::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, handle_);
    }
}
