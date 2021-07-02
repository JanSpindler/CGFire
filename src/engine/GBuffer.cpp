//
// Created by JS on 18/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/render/GBuffer.hpp"
#include "engine/Util.hpp"
#include <iostream>

namespace en
{
    GBuffer::GBuffer(int32_t width, int32_t height) :
        width_(0),
        height_(0)
    {
        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

        // Pos
        glGenTextures(1, &posTex_);
        glBindTexture(GL_TEXTURE_2D, posTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex_, 0);

        // Normal
        glGenTextures(1, &normalTex_);
        glBindTexture(GL_TEXTURE_2D, normalTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTex_, 0);

        // Diffuse color
        glGenTextures(1, &diffuseTex_);
        glBindTexture(GL_TEXTURE_2D, diffuseTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, diffuseTex_, 0);

        // Specular color (3 color + 1 shininess)
        glGenTextures(1, &specularTex_);
        glBindTexture(GL_TEXTURE_2D, specularTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, specularTex_, 0);

        // Add color attachments to frambuffer
        std::vector<uint32_t> colorAttachments = {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3
        };
        glDrawBuffers(colorAttachments.size(), colorAttachments.data());

        // Render depth buffer
        glGenRenderbuffers(1, &depthBuffer_);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);

        // Check
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Log::Error("Failed to completely construct g buffer", true);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    };

    GBuffer::~GBuffer()
    {
        glDeleteTextures(1, &posTex_);
        glDeleteTextures(1, &normalTex_);
        glDeleteTextures(1, &diffuseTex_);
        glDeleteTextures(1, &specularTex_);
        glDeleteRenderbuffers(1, &depthBuffer_);
        glDeleteFramebuffers(1, &fbo_);
    }

    void GBuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GBuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::Resize(int32_t width, int32_t height)
    {
        if (width == 0 || height == 0)
            return;

        if (width == width_ && height == height_)
            return;

        glBindTexture(GL_TEXTURE_2D, posTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

        glBindTexture(GL_TEXTURE_2D, normalTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

        glBindTexture(GL_TEXTURE_2D, diffuseTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, specularTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        width_ = width;
        height_ = height;
    }
    void GBuffer::UseTexturesSSAO(const GLProgram *program) const{
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, posTex_);
        program->SetUniformI("pos_tex", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTex_);
        program->SetUniformI("normal_tex", 1);
    }

    void GBuffer::UseTextures(const GLProgram* program) const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, posTex_);
        program->SetUniformI("pos_tex", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTex_);
        program->SetUniformI("normal_tex", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, diffuseTex_);
        program->SetUniformI("diffuse_tex", 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, specularTex_);
        program->SetUniformI("specular_tex", 3);
    }

    void GBuffer::CopyDepthBufToDefaultFb() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
