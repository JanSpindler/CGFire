//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_RENDERER_HPP
#define CGFIRE_RENDERER_HPP

#include "Light.hpp"
#include <vector>
#include "GLShader.hpp"
#include "Model.hpp"

namespace en
{
    class Renderer
    {
    public:
        Renderer(const GLProgram* program);
        ~Renderer();

        void Render(const Model* model);

        void SetDirectionalLight(DirectionalLight light);
        void AddPointLight(PointLight light);
        void RemovePointLight(PointLight light);

    private:
        const GLProgram* program_;
        DirectionalLight dirLight_;
        std::vector<PointLight> pointLights_;
    };
}

#endif //CGFIRE_RENDERER_HPP
