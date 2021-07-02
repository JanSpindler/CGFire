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
#include "ReflectiveMap.hpp"
#include "ssao/ssao.h"

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

        void AddSplineRenderObj(const RenderObj* renderObj);
        void RemoveSplineRenderObj(const RenderObj* renderObj);

        void AddReflectiveRenderObj(const RenderObj* renderObj);
        void RemoveReflectiveRenderObj(const RenderObj* renderObj);

        void SetDirLight(const DirLight* dirLight);

        void AddPointLight(const PointLight* pointLight);
        void RemovePointLight(const PointLight* pointLight);

        void SetSkyboxTex(const GLSkyboxTex* skyboxTex);

        /**Removes all the objects, i.e. clears the vector lists (but not the DirLight or Skybox)*/
        void RemoveAllObjects();
    private:
        const GLProgram* geometryProgram_;
        const GLProgram* lightingProgram_;
        const GLProgram* fixedColorProgram_;
        const GLProgram* toEnvMapProgram_;
        const GLProgram* dirShadowProgram_;
        const GLProgram* pointShadowProgram_;
        const GLProgram* reflectiveProgram_;
        const GLProgram* skyboxProgram_;
        const GLProgram* SSAOProgram_;
        const GLProgram* SSAOBlurProgram_;

        std::vector<const RenderObj*> standardRenderObjs_;
        std::vector<const RenderObj*> fixedColorRenderObjs_;
        std::vector<const RenderObj*> splineRenderObjs_;
        std::vector<const RenderObj*> reflectiveRenderObjs_;

        std::vector<ReflectiveMap> reflectiveMaps_;

        const DirLight* dirLight_;
        std::vector<const PointLight*> pointLights_;
        const GLSkyboxTex* skyboxTex_;

        ssao ssao_;

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
        void RenderSplines(const float* viewMat, const float* projMat) const;
        void RenderReflectiveMaps() const;
        void RenderReflectiveObj(glm::vec3 camPos, const float* viewMat, const float* projMat) const;
        void RenderSkybox(const float* viewMat, const float* projMat) const;
    };
}

#endif //CGFIRE_SCENERENDERER_HPP
