//
// Created by JS on 27/05/2021.
//

#ifndef CGFIRE_RENDERABLE_HPP
#define CGFIRE_RENDERABLE_HPP

#include "engine/Render/GLShader.hpp"
#include <glm/glm.hpp>

namespace en
{
    class Renderable
    {
    public:
        virtual void RenderPosOnly(const GLProgram* program) const = 0;
        virtual void RenderDiffuse(const GLProgram* program) const = 0;
        virtual void RenderAll(const GLProgram* program) const = 0;
    };

    class RenderObj
    {
    public:
        glm::mat4 t_;

        RenderObj(const Renderable* renderable);

        void RenderPosOnly(const GLProgram* program) const;
        void RenderDiffuse(const GLProgram* program) const;
        void RenderAll(const GLProgram* program) const;

        glm::vec3 GetPos() const;

    private:
        const Renderable* renderable_;

        void SetMatrices(const GLProgram* program) const;
    };
}

#endif //CGFIRE_RENDERABLE_HPP
