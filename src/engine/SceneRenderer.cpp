//
// Created by JS on 18/06/2021.
//

#include "engine/render/SceneRenderer.hpp"

namespace en
{
    SceneRenderer::SceneRenderer(
            const Camera* cam,
            const GLProgram* geometryProgram,
            const GLProgram* lightingProgram,
            const GLProgram* fixedColorProgram,
            const GLProgram* dirShadowProgram,
            const GLProgram* pointShadowProgram,
            int32_t width,
            int32_t height) :
            cam_(cam),
            geometryProgram_(geometryProgram),
            lightingProgram_(lightingProgram),
            fixedColorProgram_(fixedColorProgram),
            dirShadowProgram_(dirShadowProgram),
            pointShadowProgram_(pointShadowProgram),
            standardRenderObjs_({}),
            fixedColorRenderObjs_({}),
            gBuffer_(width, height)
    {
    }

    void SceneRenderer::Render() const
    {
        glm::mat4 viewMat = cam_->GetViewMat();
        glm::mat4 projMat = cam_->GetProjMat();

        // Render shadow textures

        // Render geometric information to g buffer
        gBuffer_.Bind();
        geometryProgram_->Use();
        geometryProgram_->SetUniformMat4("view_mat", false, &viewMat[0][0]);
        geometryProgram_->SetUniformMat4("proj_mat", false, &projMat[0][0]);
        for (const RenderObj* renderObj : standardRenderObjs_)
        {
        }

        // Use g buffer to render final image with lighting information
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
}
