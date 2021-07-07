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

        virtual void Render(const GLProgram* program) const = 0; // Deprecated
    };

    class RenderObj : public Renderable
    {
    public:
        glm::mat4 t_;

        RenderObj();

        void RenderPosOnly(const GLProgram* program) const override;
        void RenderDiffuse(const GLProgram* program) const override;
        void RenderAll(const GLProgram* program) const override;

        void Render(const GLProgram* program) const override; // Deprecated

        virtual glm::vec3 GetPos() const;

    protected:
        void SetMatrices(const GLProgram* program) const;

    private:
    };
}

#endif //CGFIRE_RENDERABLE_HPP
