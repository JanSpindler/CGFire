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

namespace en
{
    class SceneRenderer
    {
    public:
        SceneRenderer(
                const Camera* cam,
                const GLProgram* geometryProgram,
                const GLProgram* lightingProgram,
                const GLProgram* fixedColorProgram,
                const GLProgram* dirShadowProgram,
                const GLProgram* pointShadowProgram,
                int32_t width,
                int32_t height);

        void Render() const;
        void Resize(int32_t width, int32_t height);

        void AddStandardRenderObj(const RenderObj* renderObj);
        void RemoveStandardRenderObj(const RenderObj* renderObj);

        void AddFixedColorRenderObj(const RenderObj* renderObj);
        void RemoveFixedColorRenderObj(const RenderObj* renderObj);

        void AddDirLight(const DirLight* dirLight);
        void RemoveDirLight(const DirLight* dirLight);

        void AddPointLight(const PointLight* pointLight);
        void RemovePointLight(const PointLight* pointLight);

    private:
        const Camera* cam_;

        const GLProgram* geometryProgram_;
        const GLProgram* lightingProgram_;
        const GLProgram* fixedColorProgram_;
        const GLProgram* dirShadowProgram_;
        const GLProgram* pointShadowProgram_;

        std::vector<const RenderObj*> standardRenderObjs_;
        std::vector<const RenderObj*> fixedColorRenderObjs_;
        std::vector<const DirLight*> dirLights_;
        std::vector<const PointLight*> pointLights_;

        GBuffer gBuffer_;
    };
}

#endif //CGFIRE_SCENERENDERER_HPP
