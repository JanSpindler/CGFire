//
// Created by JS on 27/05/2021.
//

#ifndef CGFIRE_RENDERABLE_HPP
#define CGFIRE_RENDERABLE_HPP

#include "GLShader.hpp"
#include <glm/glm.hpp>

namespace en
{
    class Renderable
    {
    public:
        virtual void Render(const GLProgram* program) const = 0;
    };

    class RenderObj
    {
    public:
        glm::mat4 t_;

        RenderObj(const Renderable* renderable);

        void Render(const GLProgram* program);

    private:
        const Renderable* renderable_;
    };
}

#endif //CGFIRE_RENDERABLE_HPP
