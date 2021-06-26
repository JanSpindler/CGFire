//
// Created by JS on 18/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/render/SceneRenderer.hpp"
#include "engine/Util.hpp"

namespace en
{
    // Helps with constructing the fullScreenVao_
    struct ScreenVertex
    {
        glm::vec3 pos;
        glm::vec2 uv;
    };

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
        ScreenVertex vertices[] = {
                { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
                { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
                { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
                { glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) }
        };

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &fullScreenVao_);
        glBindVertexArray(fullScreenVao_);

        uint32_t vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(ScreenVertex), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (void*) offsetof(ScreenVertex, ScreenVertex::pos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (void*) offsetof(ScreenVertex, ScreenVertex::uv));

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

        RenderDirShadows();
        RenderPointShadows();
        RenderToGBuffer(&viewMat[0][0], &projMat[0][0]);
        RenderLighting(window);
    }

    void SceneRenderer::Resize(int32_t width, int32_t height)
    {
        gBuffer_ = GBuffer(width, height);
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

    void SceneRenderer::AddDirLight(const DirLight* dirLight)
    {
        RemoveDirLight(dirLight);
        dirLights_.push_back(dirLight);
    }

    void SceneRenderer::RemoveDirLight(const DirLight* dirLight)
    {
        for (uint32_t i = 0; i < dirLights_.size(); i++)
        {
            if (dirLights_[i] == dirLight)
            {
                dirLights_.erase(dirLights_.begin() + i);
                return;
            }
        }
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

    void SceneRenderer::RenderDirShadows() const
    {
        dirShadowProgram_->Use();
        glm::mat4 lightMat;
        for (const DirLight* dirLight : dirLights_)
        {
            lightMat = dirLight->GetLightMat();
            dirShadowProgram_->SetUniformMat4("light_mat", false, &lightMat[0][0]);

            dirLight->BindShadowBuffer();
            for (const RenderObj* standardRenderObj : standardRenderObjs_)
                standardRenderObj->RenderToShadowMap(dirShadowProgram_);
            dirLight->UnbindShadowBuffer();
        }
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
            for (const RenderObj* standardRenderObj : standardRenderObjs_)
                standardRenderObj->RenderToShadowMap(pointShadowProgram_);
            pointLight->UnbindShadowBuffer();
        }
    }

    void SceneRenderer::RenderToGBuffer(const float* viewMat, const float* projMat) const
    {
        gBuffer_.Bind();

        geometryProgram_->Use();
        geometryProgram_->SetUniformMat4("view_mat", false, viewMat);
        geometryProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (const RenderObj* standardRenderObj : standardRenderObjs_)
            standardRenderObj->RenderToGBuffer(geometryProgram_);

        gBuffer_.Unbind();
    }

    void SceneRenderer::RenderLighting(const Window* window) const
    {
        window->UseViewport();
        gBuffer_.UseTextures(lightingProgram_);

        glBindVertexArray(fullScreenVao_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}
