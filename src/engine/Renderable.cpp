//
// Created by JS on 27/05/2021.
//

#include "engine/Render/Renderable.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace en
{
    RenderObj::RenderObj(const Renderable *renderable)
    {
        t_ = glm::identity<glm::mat4>();
        renderable_ = renderable;
    }

    void RenderObj::RenderPosOnly(const GLProgram *program) const
    {
        SetMatrices(program);
        renderable_->RenderPosOnly(program);
    }

    void RenderObj::RenderDiffuse(const GLProgram *program) const
    {
        SetMatrices(program);
        renderable_->RenderDiffuse(program);
    }

    void RenderObj::RenderAll(const GLProgram *program) const
    {
        SetMatrices(program);
        renderable_->RenderAll(program);
    }

    glm::vec3 RenderObj::GetPos() const
    {
        return glm::vec3(t_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void RenderObj::SetMatrices(const GLProgram *program) const
    {
        program->SetUniformMat4("model_mat", false, &t_[0][0]);
        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(t_)));
        program->SetUniformMat3("normal_mat", false, &normalMat[0][0]);
    }
}
