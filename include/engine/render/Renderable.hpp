//
// Created by JS on 27/05/2021.
//

#pragma once
#ifndef CGFIRE_RENDERABLE_HPP
#define CGFIRE_RENDERABLE_HPP

#include "engine/Render/GLShader.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

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

    //Be aware:
    // Model can be: Standard, FixedColor, Reflective
    // Sceletal can be: Sceletal, fixedColor, Reflective
    // Spline can be: Spline only (automatically also FixedColor)
    enum class RenderObjType {
        Skeletal, Standard, FixedColor, Spline, Reflective
    };


    static int NumRenderObjs = 0; //for ID creation
    class RenderObj : public Renderable
    {
    public:
        int ID;
        glm::vec3 Position = glm::vec3(0.f, 0.f, 0.f);
        //glm::vec3 EulerAngles = glm::vec3(0.f, 0.f, 0.f);
        glm::quat Quaternion = glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f));
        glm::vec3 Scaling = glm::vec3(1.f, 1.f, 1.f);
        glm::mat4 prevM;
        bool blur = false;

        const glm::mat4& GetTransform() { return t_; }

        RenderObj();

        void RenderPosOnly(const GLProgram* program) override;
        void RenderDiffuse(const GLProgram* program) override;
        void RenderAll(const GLProgram* program) override;

        void Render(const GLProgram* program) override; // Deprecated

        virtual glm::vec3 GetPos();

        void SetName(const std::string& name){ name_ = name; }
        const std::string& GetName(){ return name_; }

        virtual bool IsRenderObjTypePossible(RenderObjType type) const = 0;
    private:
        glm::mat4 t_;

        void SetMatrices(const GLProgram* program);
    protected:
        std::string name_= "noname";
    };
}

#endif //CGFIRE_RENDERABLE_HPP
