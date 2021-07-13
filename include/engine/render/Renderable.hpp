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
        virtual void RenderPosOnly(const GLProgram* program) = 0;
        virtual void RenderDiffuse(const GLProgram* program) = 0;
        virtual void RenderAll(const GLProgram* program) = 0;

        virtual void Render(const GLProgram* program) = 0; // Deprecated
    };

    static int NumRenderObjs = 0;
    class RenderObj : public Renderable
    {
    public:
        int ID;
        glm::vec3 Position = glm::vec3(0.f, 0.f, 0.f);
        float RotationAngle = 0.f;
        glm::vec3 RotationAxis = glm::vec3(0.f, 1.f, 0.f);
        glm::vec3 Scaling = glm::vec3(1.f, 1.f, 1.f);
        glm::mat4 prevM;
        bool blur = false;

        const glm::mat4& GetTransform() { return t_; }

        RenderObj(const std::string& name = "noname");

        void RenderPosOnly(const GLProgram* program) override;
        void RenderDiffuse(const GLProgram* program) override;
        void RenderAll(const GLProgram* program) override;

        void Render(const GLProgram* program) override; // Deprecated

        virtual glm::vec3 GetPos();

        const std::string& GetName(){ return name_; }

        virtual void OnImGuiRender();

    protected:
        std::string name_;

        void SetMatrices(const GLProgram* program);

    private:
        glm::mat4 t_;
    };
}

#endif //CGFIRE_RENDERABLE_HPP
