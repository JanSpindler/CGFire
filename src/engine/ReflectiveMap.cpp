//
// Created by JS on 28/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/render/ReflectiveMap.hpp"
#include "engine/Util.hpp"
#include <glm/gtx/transform.hpp>

namespace en
{
    ReflectiveMap::ReflectiveMap(uint32_t size) :
        size_(size)
    {
        Log::Info("Creating Reflective Map");

        // Create framebuffer
        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

        // Create cube map
        glGenTextures(1, &cubeMap_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap_);

        for (uint32_t i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Create renderbuffer
        glGenRenderbuffers(1, &depthBuffer_);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size, size);

        // Attach buffer to framebuffer
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);

        // Check framebuffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Log::Error("Failed to completely construct reflective map", true);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ReflectiveMap::~ReflectiveMap()
    {
    }

    void ReflectiveMap::BindFbo() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    }

    void ReflectiveMap::UnbindFbo() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ReflectiveMap::BindCubeMap() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap_);
    }

    void ReflectiveMap::BindCubeMapFace(uint8_t i) const
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap_, 0);
    }

    uint32_t ReflectiveMap::GetSize() const
    {
        return size_;
    }

    glm::mat4 ReflectiveMap::GetProjMat() const
    {
        return glm::perspective(glm::radians(90.0f), 1.0f, 0.25f, 256.0f);
    }

    std::vector<glm::mat4> ReflectiveMap::GetViewMats(glm::vec3 pos) const
    {
        std::vector<glm::mat4> viewMats = {
                glm::lookAt(pos, pos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                glm::lookAt(pos, pos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                glm::lookAt(pos, pos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
                glm::lookAt(pos, pos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
                glm::lookAt(pos, pos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))
        };
        return viewMats;
    }
}
