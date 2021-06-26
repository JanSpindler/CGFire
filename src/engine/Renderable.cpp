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

    void RenderObj::Render(const GLProgram *program) const
    {
        SetModelMat(program);
        renderable_->Render(program);
    }

    void RenderObj::RenderToGBuffer(const GLProgram *program) const
    {
        SetModelMat(program);
        renderable_->RenderToGBuffer(program);
    }

    void RenderObj::RenderToShadowMap(const GLProgram *program) const
    {
        SetModelMat(program);
        renderable_->RenderToShadowMap(program);
    }

    void RenderObj::RenderFixedColor(const GLProgram *program) const
    {
        SetModelMat(program);
        renderable_->RenderFixedColor(program);
    }

    glm::vec3 RenderObj::GetPos() const
    {
        return glm::vec3(t_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void RenderObj::SetModelMat(const GLProgram *program) const
    {
        program->SetUniformMat4("model_mat", false, &t_[0][0]);
    }
}
