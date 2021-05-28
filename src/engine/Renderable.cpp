//
// Created by JS on 27/05/2021.
//

#include "engine/Renderable.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace en
{
    RenderObj::RenderObj(const Renderable *renderable)
    {
        t_ = glm::identity<glm::mat4>();
        renderable_ = renderable;
    }

    void RenderObj::Render(const GLProgram *program)
    {
        program->SetUniformMat4("model_mat", false, &t_[0][0]);
        renderable_->Render(program);
    }
}