//
// Created by JS on 31/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/render/GLDepthMap.hpp"

namespace en
{
    GLDepthTex::GLDepthTex(int width, int height) :
        GLTexture()
    {
        glBindTexture(GL_TEXTURE_2D, handle_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void GLDepthTex::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, handle_);
    }

    void GLDepthTex::BindToFramebuffer() const
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, handle_, 0);
    }

    GLDepthMap::GLDepthMap() :
        tex_(GLDepthTex(WIDTH_, HEIGHT_))
    {
        glGenFramebuffers(1, &fbo_);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        tex_.BindToFramebuffer();
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GLDepthMap::Bind() const
    {
        glViewport(0, 0, WIDTH_, HEIGHT_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void GLDepthMap::UnbindFramebuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
