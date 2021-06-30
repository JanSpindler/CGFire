//
// Created by JS on 30/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/render/Mirror.hpp"
#include <glm/gtx/transform.hpp>
#include "engine/Util.hpp"

namespace en
{
    Mirror::Mirror(const RenderObj *renderObj, glm::vec3 normal, uint32_t size) :
        renderObj_(renderObj_),
        normal_(normal)
    {
        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

        glGenTextures(1, &colorTex_);
        glBindTexture(GL_TEXTURE_2D, colorTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glGenRenderbuffers(1, &depthBuffer_);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size, size);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Log::Error("Failed to completely construct mirror buffer", true);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Mirror::BindFbo() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    }

    void Mirror::UnbindFbo() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glm::mat4 Mirror::GetViewMat(const Camera* camera) const
    {
        glm::vec3 camPos = camera->GetPos();
        glm::vec3 camViewDir = camera->GetViewDir();

        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(renderObj_->t_)));
        glm::vec3 normalVec = glm::normalize(normalMat * normal_);

        glm::vec3 viewDir = glm::reflect(camViewDir, normalVec);
        glm::vec3 pos = renderObj_->GetPos() + glm::reflect(camPos - renderObj_->GetPos(), normalVec);

        return glm::lookAt(pos, pos + viewDir, glm::vec3(0.0f, -1.0f, 0.0f));
    }
}
