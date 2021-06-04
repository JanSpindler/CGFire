//
// Created by JS on 26/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Render/Light.hpp"
#include "engine/Util.hpp"
#include <glm/gtx/transform.hpp>

#define SHADOW_TEX_WIDTH 4096
#define SHADOW_TEX_HEIGHT SHADOW_TEX_WIDTH

namespace en
{
    GLDepthTex::GLDepthTex(int width, int height) :
            GLTexture()
    {
        glBindTexture(GL_TEXTURE_2D, handle_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void GLDepthTex::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, handle_);
    }

    void GLDepthTex::BindToFramebuffer() const
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, handle_, 0);
    }

    DirLight::DirLight(glm::vec3 dir, glm::vec3 color) :
            depthTex_(SHADOW_TEX_WIDTH, SHADOW_TEX_HEIGHT)
    {
        dir_ = dir;
        color_ = color;

        glGenFramebuffers(1, &shadowFbo_);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);
        depthTex_.BindToFramebuffer();
        glDrawBuffer(GL_NONE);
        glDrawBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DirLight::Use(const GLProgram *program) const
    {
        program->SetUniformVec3f("dir_light_dir", dir_);
        program->SetUniformVec3f("dir_light_color", color_);
    }

    void DirLight::SetDir(glm::vec3 dir)
    {
        dir_ = dir;
    }

    void DirLight::SetColor(glm::vec3 color)
    {
        color_ = color;
    }

    void DirLight::UseShadow(const GLProgram *program)
    {
        glActiveTexture(GL_TEXTURE0 + 1);
        depthTex_.Bind();
        program->SetUniformI("shadow_tex", 1);
    }

    glm::mat4 DirLight::GetLightMat() const
    {
        glm::mat4 viewMat = glm::lookAt(-dir_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        float size = 20.0f;
        glm::mat4 projMat = glm::ortho(-size, size, -size, size, 0.01f, 1000.0f);
        return projMat * viewMat;
    }

    void DirLight::BindShadowBuffer() const
    {
        glViewport(0, 0, SHADOW_TEX_WIDTH, SHADOW_TEX_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void DirLight::UnbindShadowBuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    PointLight::PointLight(float strength)
    {
        strength_ = strength;
    }

    float PointLight::GetStrength() const
    {
        return strength_;
    }

    PointLightBatch::PointLightBatch(const std::vector<const PointLight*>& pointLights)
    {
        pointLights_ = pointLights;
    }

    void PointLightBatch::Use(const GLProgram *program)
    {
        unsigned int pointLightCount = pointLights_.size();
        program->SetUniformUI("point_light_count", pointLightCount);
        for (unsigned int i = 0; i < pointLightCount; i++)
        {
            const PointLight* pointLight = pointLights_[i];
            std::string indexStr = std::to_string(i);
            program->SetUniformVec3f("point_light_pos[" + indexStr + "]", pointLight->GetPos());
            program->SetUniformVec3f("point_light_color[" + indexStr + "]", pointLight->GetColor());
            program->SetUniformF("point_light_strength[" + indexStr + "]", pointLight->GetStrength());
        }
    }

    void PointLightBatch::AddPointLight(const PointLight *pointLight)
    {
        if (std::find(pointLights_.begin(), pointLights_.end(), pointLight) != pointLights_.end())
            pointLights_.push_back(pointLight);
    }

    void PointLightBatch::RemovePointLight(const PointLight *pointLight)
    {
        auto it = std::find(pointLights_.begin(), pointLights_.end(), pointLight);
        if (it != pointLights_.end())
            pointLights_.erase(it);
    }
}
