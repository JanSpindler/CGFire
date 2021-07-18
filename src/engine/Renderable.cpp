//
// Created by JS on 27/05/2021.
//

#include "engine/render/Renderable.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <framework/imgui_util.hpp>

namespace en
{
    RenderObj::RenderObj()
    {
        t_ = glm::identity<glm::mat4>();
        prevM = t_;
        ID = NumRenderObjs++;
    }

    void RenderObj::RenderPosOnly(const GLProgram *program)
    {
        SetMatrices(program);
    }

    void RenderObj::RenderDiffuse(const GLProgram *program)
    {
        SetMatrices(program);
    }

    void RenderObj::RenderAll(const GLProgram *program)
    {
        SetMatrices(program);
    }

    void RenderObj::Render(const GLProgram *program) // Deprecated
    {
        SetMatrices(program);
    }

    glm::vec3 RenderObj::GetPos()
    {
        return glm::vec3(t_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void RenderObj::SetMatrices(const GLProgram *program)
    {

        auto trans = glm::translate(glm::mat4(1.f), Position);
        auto rot = glm::eulerAngleYXZ(EulerAngles.y, EulerAngles.x, EulerAngles.z);
        auto scale = glm::scale(glm::mat4(1.f), Scaling);
        t_ =  trans * rot * scale;

        program->SetUniformMat4("model_mat", false, &t_[0][0]);
        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(t_)));
        program->SetUniformMat3("normal_mat", false, &normalMat[0][0]);
        program->SetUniformMat4("prevM", false, &prevM[0][0]);
        prevM = t_;
    }
}
