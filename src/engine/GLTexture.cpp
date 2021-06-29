//
// Created by JS on 26/05/2021.
//

#include "engine/gr_include.hpp"
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "engine/Render/GLTexture.hpp"
#include "engine/Util.hpp"
#include "engine/config.hpp"

namespace en
{
    GLTexture::GLTexture()
    {
        glGenTextures(1, &handle_);
    }

    GLTexture::~GLTexture()
    {
        glDeleteTextures(1, &handle_);
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

    void GLPictureTex::BindTex() const
    {
        glBindTexture(GL_TEXTURE_2D, handle_);
    }

    GLDepthTex::GLDepthTex(int width, int height) :
            GLTexture()
    {
        glBindTexture(GL_TEXTURE_2D, handle_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    void GLDepthTex::BindTex() const
    {
        glBindTexture(GL_TEXTURE_2D, handle_);
    }

    void GLDepthTex::BindToFramebuffer() const
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, handle_, 0);
    }

    GLDepthCubeMap::GLDepthCubeMap(unsigned int width, unsigned int height) :
        GLTexture()
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);

        for (unsigned int i = 0; i < 6; i++)
            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_DEPTH_COMPONENT,
                    width,
                    height,
                    0,
                    GL_DEPTH_COMPONENT,
                    GL_FLOAT,
                    nullptr);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void GLDepthCubeMap::BindTex() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);
    }

    void GLDepthCubeMap::BindToFramebuffer() const
    {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, handle_, 0);
    }

    GLSkyboxTex::GLSkyboxTex(const std::string& dirPath, const std::string& fileExtension, bool flipUv) :
        GLTexture()
    {
        std::vector<std::string> subFiles = {
                "/right",
                "/left",
                "/top",
                "/bottom",
                "/front",
                "/back"
        };

        glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);

        stbi_set_flip_vertically_on_load(flipUv);

        int32_t width, height, channelCount;
        uint8_t* data;
        std::string totalFileName;
        for (uint32_t i = 0; i < 6; i++)
        {
            totalFileName = DATA_ROOT + dirPath + subFiles[i] + fileExtension;
            data = stbi_load(totalFileName.c_str(), &width, &height, &channelCount, 0);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void GLSkyboxTex::BindTex() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);
    }
}
