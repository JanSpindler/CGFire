//
// Created by JS on 26/05/2021.
//

#include "engine/Render/Light.hpp"
#include "engine/Util.hpp"
#include <glm/gtx/transform.hpp>

namespace en
{
    ShadowLight::ShadowLight()
    {
        depthMap_ = GLDepthMap();
    }

    void ShadowLight::BindDepthMap() const
    {
        depthMap_.Bind();
    }

    void ShadowLight::UnbindDepthMap() const
    {
        depthMap_.UnbindFramebuffer();
    }

    DirLight::DirLight(glm::vec3 dir, glm::vec3 color) :
        ShadowLight()
    {
        dir_ = dir;
        color_ = color;
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

    glm::mat4 DirLight::GetShadowViewMat() const
    {
        return glm::lookAt(-dir_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 DirLight::GetShadowProjMat() const
    {
        return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 1000.0f);
    }

    PointLight::PointLight(float strength) :
        ShadowLight()
    {
        strength_ = strength;
    }

    float PointLight::GetStrength() const
    {
        return strength_;
    }

    glm::mat4 PointLight::GetShadowViewMat() const
    {
        return glm::identity<glm::mat4>();
    }

    glm::mat4 PointLight::GetShadowProjMat() const
    {
        return glm::identity<glm::mat4>();
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
