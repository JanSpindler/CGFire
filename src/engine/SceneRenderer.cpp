//
// Created by JS on 18/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/render/SceneRenderer.hpp"
#include "engine/Util.hpp"
#include "engine/config.hpp"

namespace en
{
    SceneRenderer::SceneRenderer(
            const GLProgram* geometryProgram,
            const GLProgram* lightingProgram,
            const GLProgram* fixedColorProgram,
            const GLProgram* dirShadowProgram,
            const GLProgram* pointShadowProgram,
            int32_t width,
            int32_t height) :
            geometryProgram_(geometryProgram),
            lightingProgram_(lightingProgram),
            fixedColorProgram_(fixedColorProgram),
            dirShadowProgram_(dirShadowProgram),
            pointShadowProgram_(pointShadowProgram),
            standardRenderObjs_({}),
            fixedColorRenderObjs_({}),
            gBuffer_(width, height)
    {
        glm::vec3 vertices[] = {
                glm::vec3(-1.0f, -1.0f, 0.0f),
                glm::vec3(1.0f, -1.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 0.0f),
                glm::vec3(-1.0f, 1.0f, 0.0f)
        };

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &fullScreenVao_);
        glBindVertexArray(fullScreenVao_);

        uint32_t vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

        uint32_t ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void SceneRenderer::Render(const Window* window, const Camera* cam) const
    {
        glm::mat4 viewMat = cam->GetViewMat();
        glm::mat4 projMat = cam->GetProjMat();

        RenderDirShadow();
        RenderPointShadows();

        window->UseViewport();
        RenderDeferredGeometry(&viewMat[0][0], &projMat[0][0]);
        RenderDeferredLighting(window, cam);
    }

    void SceneRenderer::Resize(int32_t width, int32_t height)
    {
        gBuffer_.Resize(width, height);
    }

    void SceneRenderer::AddStandardRenderObj(const RenderObj* renderObj)
    {
        RemoveStandardRenderObj(renderObj);
        standardRenderObjs_.push_back(renderObj);
    }

    void SceneRenderer::RemoveStandardRenderObj(const RenderObj* renderObj)
    {
        for (uint32_t i = 0; i < standardRenderObjs_.size(); i++)
        {
            if (standardRenderObjs_[i] == renderObj)
            {
                standardRenderObjs_.erase(standardRenderObjs_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::AddFixedColorRenderObj(const RenderObj* renderObj)
    {
        RemoveFixedColorRenderObj(renderObj);
        fixedColorRenderObjs_.push_back(renderObj);
    }

    void SceneRenderer::RemoveFixedColorRenderObj(const RenderObj* renderObj)
    {
        for (uint32_t i = 0; i < fixedColorRenderObjs_.size(); i++)
        {
            if (fixedColorRenderObjs_[i] == renderObj)
            {
                fixedColorRenderObjs_.erase(fixedColorRenderObjs_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::SetDirLight(const DirLight *dirLight)
    {
        dirLight_ = dirLight;
    }

    void SceneRenderer::AddPointLight(const PointLight* pointLight)
    {
        RemovePointLight(pointLight);
        pointLights_.push_back(pointLight);
    }

    void SceneRenderer::RemovePointLight(const PointLight* pointLight)
    {
        for (uint32_t i = 0; i < pointLights_.size(); i++)
        {
            if (pointLights_[i] == pointLight)
            {
                pointLights_.erase(pointLights_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::RenderDirShadow() const
    {
        dirShadowProgram_->Use();
        glm::mat4 lightMat = dirLight_->GetLightMat();
        dirShadowProgram_->SetUniformMat4("light_mat", false, &lightMat[0][0]);

        dirLight_->BindShadowBuffer();
        for (const RenderObj* renderObj : standardRenderObjs_)
            renderObj->RenderToShadowMap(dirShadowProgram_);
        for (const RenderObj* renderObj : fixedColorRenderObjs_)
            renderObj->RenderToShadowMap(dirShadowProgram_);
        dirLight_->UnbindShadowBuffer();
    }

    void SceneRenderer::RenderPointShadows() const
    {
        pointShadowProgram_->Use();
        std::vector<glm::mat4> lightMats;
        for (const PointLight* pointLight : pointLights_)
        {
            lightMats = pointLight->GetLightMats();
            for (uint32_t i = 0; i < 6; i++)
                pointShadowProgram_->SetUniformMat4("light_mats[" + std::to_string(i) + "]", false, &lightMats[i][0][0]);
            pointShadowProgram_->SetUniformVec3f("light_pos", pointLight->GetPos());

            pointLight->BindShadowBuffer();
            for (const RenderObj* renderObj : standardRenderObjs_)
                renderObj->RenderToShadowMap(pointShadowProgram_);
            for (const RenderObj* renderObj : fixedColorRenderObjs_)
                renderObj->RenderToShadowMap(pointShadowProgram_);
            pointLight->UnbindShadowBuffer();
        }
    }

    void SceneRenderer::RenderDeferredGeometry(const float* viewMat, const float* projMat) const
    {
        gBuffer_.Bind();

        geometryProgram_->Use();
        geometryProgram_->SetUniformMat4("view_mat", false, viewMat);
        geometryProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (const RenderObj* renderObj : standardRenderObjs_)
            renderObj->RenderToGBuffer(geometryProgram_);

        gBuffer_.Unbind();
    }

    void SceneRenderer::RenderDeferredLighting(const Window* window, const Camera* cam) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        lightingProgram_->Use();

        gBuffer_.UseTextures(lightingProgram_);
        lightingProgram_->SetUniformVec3f("cam_pos", cam->GetPos());

        glm::mat4 dirLightMat = dirLight_->GetLightMat();
        lightingProgram_->SetUniformMat4("dir_light_mat", false, &dirLightMat[0][0]);
        dirLight_->Use(lightingProgram_);
        dirLight_->UseShadow(lightingProgram_);

        const int32_t pointLightCount = std::min((int)pointLights_.size(), 24);
        lightingProgram_->SetUniformI("point_light_count", pointLightCount);
        for (uint32_t i = 0; i < pointLightCount; i++)
        {
            const PointLight* pointLight = pointLights_[i];
            pointLight->Use(lightingProgram_, i);
            pointLight->UseShadow(lightingProgram_, i);
        }

        glBindVertexArray(fullScreenVao_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void SceneRenderer::RenderFixedColor(const float* viewMat, const float* projMat) const
    {
        fixedColorProgram_->Use();
        fixedColorProgram_->SetUniformMat4("view_mat", false, viewMat);
        fixedColorProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (const RenderObj* renderObj : fixedColorRenderObjs_)
            renderObj->RenderFixedColor(fixedColorProgram_);
    }
}
