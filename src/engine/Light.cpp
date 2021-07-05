//
// Created by JS on 26/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Render/Light.hpp"
#include "engine/Util.hpp"
#include <glm/gtx/transform.hpp>
#include <limits>

namespace en
{
    DirLight::DirLight(glm::vec3 dir, glm::vec3 color, uint32_t width, uint32_t height) :
            depthTex_(width, height),
            width_(width),
            height_(height)
    {
        dir_ = glm::normalize(dir);
        color_ = color;

        glGenFramebuffers(1, &shadowFbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);

        glGenTextures(1, &esmTex_);
        glBindTexture(GL_TEXTURE_2D, esmTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float floatLimit = std::numeric_limits<float>::max();
        float borderColor[] = { floatLimit, floatLimit, floatLimit, floatLimit };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, esmTex_, 0);

        std::vector<uint32_t> drawBuffers = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());

        depthTex_.BindToFramebuffer();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // This texture is used as buffer
        glGenTextures(1, &esmTmpTex_);
        glBindTexture(GL_TEXTURE_2D, esmTmpTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void DirLight::Use(const GLProgram *program) const
    {
        program->SetUniformVec3f("dir_light_dir", dir_);
        program->SetUniformVec3f("dir_light_color", color_);
    }

    void DirLight::SetDir(glm::vec3 dir)
    {
        dir_ = glm::normalize(dir);
    }

    void DirLight::SetColor(glm::vec3 color)
    {
        color_ = color;
    }

    uint32_t DirLight::GetWidth() const
    {
        return width_;
    }

    uint32_t DirLight::GetHeight() const
    {
        return height_;
    }

    void DirLight::UseShadow(const GLProgram *program) const
    {
        glActiveTexture(GL_TEXTURE4);
        depthTex_.BindTex();
        program->SetUniformI("dir_shadow_tex", 4);
    }

    glm::mat4 DirLight::GetLightMat() const
    {
        float maxDepth = 1000.0f;
        glm::mat4 viewMat = glm::lookAt(-dir_ * maxDepth / 2.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        float size = 128.0f;
        glm::mat4 projMat = glm::ortho(-size, size, -size, size, 0.01f, maxDepth);
        return projMat * viewMat;
    }

    void DirLight::BindShadowBuffer() const
    {
        glViewport(0, 0, width_, height_);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    void DirLight::UnbindShadowBuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DirLight::BindEsmTex() const
    {
        glBindTexture(GL_TEXTURE_2D, esmTex_);
    }

    void DirLight::PrepareGauss5(const GLProgram *gauss5Program) const
    {
        float pixelWidth = 1.0f / (float) width_;
        float pixelHeight = 1.0f / (float) height_;

        // Copy esm texture
        glBindTexture(GL_TEXTURE_2D, esmTmpTex_);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width_, height_);

        // Setup program
        gauss5Program->Use();
        gauss5Program->SetUniformF("pixel_width", pixelWidth);
        gauss5Program->SetUniformF("pixel_height", pixelHeight);
        glActiveTexture(GL_TEXTURE0);
        gauss5Program->SetUniformI("og_tex", 0);
    }

    PointLight::PointLight(float strength, uint32_t width, uint32_t height) :
            depthCubeMap_(width, height),
            strength_(strength),
            width_(width),
            height_(height)
    {
        // Framebuffer
        glGenFramebuffers(1, &shadowFbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);

        depthCubeMap_.BindToFramebuffer();

        // Esm cube map
        /*glGenTextures(1, &esmCubeMap_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, esmCubeMap_);
        for (uint32_t i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, esmCubeMap_, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);*/

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Esm tmp tex
        /*glGenTextures(1, &esmTmpTex_);
        glBindTexture(GL_TEXTURE_2D, esmTmpTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
    }

    void PointLight::Use(const GLProgram *program, unsigned int index) const
    {
        std::string indexStr = std::to_string(index);
        program->SetUniformVec3f("point_light_pos[" + indexStr + "]", GetPos());
        program->SetUniformVec3f("point_light_color[" + indexStr + "]", GetColor());
        program->SetUniformF("point_light_strength[" + indexStr + "]", GetStrength());
    }

    float PointLight::GetStrength() const
    {
        return strength_;
    }

    void PointLight::UseShadow(const GLProgram* program, unsigned int index) const
    {
        int32_t texIndex = 10 + index;
        glActiveTexture(GL_TEXTURE0 + texIndex);
        depthCubeMap_.BindTex();
        program->SetUniformI("point_shadow_tex" + std::to_string(index), texIndex);
    }

    std::vector<glm::mat4> PointLight::GetLightMats() const
    {
        float aspectRatio = (float) width_ / (float) height_;
        glm::mat4 projMat = glm::perspective(glm::radians(90.0f), aspectRatio, 0.25f, 1024.0f);
        glm::vec3 pos = GetPos();

        std::vector<glm::mat4> lightMats = {
                projMat * glm::lookAt(pos, pos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projMat * glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projMat * glm::lookAt(pos, pos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                projMat * glm::lookAt(pos, pos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
                projMat * glm::lookAt(pos, pos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
                projMat * glm::lookAt(pos, pos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))
        };
        return lightMats;
    }

    void PointLight::BindShadowBuffer() const
    {
        glViewport(0, 0, width_, height_);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void PointLight::UnbindShadowBuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /*void PointLight::BindEsmCubeMap() const
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, esmCubeMap_);
    }*/
}
