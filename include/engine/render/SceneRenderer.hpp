//
// Created by JS on 18/06/2021.
//

#ifndef CGFIRE_SCENERENDERER_HPP
#define CGFIRE_SCENERENDERER_HPP

#include <vector>
#include "engine/Camera.hpp"
#include "GLShader.hpp"
#include "Renderable.hpp"
#include "Light.hpp"
#include "GBuffer.hpp"
#include "../Window.hpp"

namespace en
{
    class SceneRenderer
    {
    public:
        SceneRenderer(int32_t width, int32_t height);

        void Render(const Window* window, const Camera* cam) const;
        void Resize(int32_t width, int32_t height);

        void AddStandardRenderObj(const RenderObj* renderObj);
        void RemoveStandardRenderObj(const RenderObj* renderObj);

        void AddFixedColorRenderObj(const RenderObj* renderObj);
        void RemoveFixedColorRenderObj(const RenderObj* renderObj);

        void SetDirLight(const DirLight* dirLight);

        void AddPointLight(const PointLight* pointLight);
        void RemovePointLight(const PointLight* pointLight);

        void SetSkyboxTex(const GLSkyboxTex* skyboxTex);

    private:
        const GLProgram* geometryProgram_;
        const GLProgram* lightingProgram_;
        const GLProgram* fixedColorProgram_;
        const GLProgram* dirShadowProgram_;
        const GLProgram* pointShadowProgram_;
        const GLProgram* skyboxProgram_;

        std::vector<const RenderObj*> standardRenderObjs_;
        std::vector<const RenderObj*> fixedColorRenderObjs_;
        const DirLight* dirLight_;
        std::vector<const PointLight*> pointLights_;
        const GLSkyboxTex* skyboxTex_;

        GBuffer gBuffer_;
        uint32_t fullScreenVao_;
        uint32_t skyboxVao_;

        void LoadPrograms();
        void CreateFullScreenVao();
        void CreateSkyboxVao();

        void RenderDirShadow() const;
        void RenderPointShadows() const;
        void RenderDeferredGeometry(const float* viewMat, const float* projMat) const;
        void RenderDeferredLighting(const Window* window, const Camera* cam) const;
        void RenderFixedColor(const float* viewMat, const float* projMat) const;
        void RenderSkybox(const float* viewMat, const float* projMat) const;
    };
}

#endif //CGFIRE_SCENERENDERER_HPP
