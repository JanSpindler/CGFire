//
// Created by JS on 27/05/2021.
//

#include "engine/render/Renderable.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <framework/imgui_util.hpp>

namespace en
{
    RenderObj::RenderObj(const std::string& name)
    : name_(name)
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
        t_ = glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), Position), RotationAngle, RotationAxis), Scaling) ;

        program->SetUniformMat4("model_mat", false, &t_[0][0]);
        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(t_)));
        program->SetUniformMat3("normal_mat", false, &normalMat[0][0]);
        program->SetUniformMat4("prevM", false, &prevM[0][0]);
        prevM = t_;
    }
    void RenderObj::OnImGuiRender(){
        std::string strID = "RO" + std::to_string(ID) + name_;
        ImGui::PushID(strID.c_str());
        if (ImGui::TreeNode(strID.c_str())) {
            ImGui::DragFloat3("Position", &Position.x, 0.25f);
            ImGui::DragFloat("RotationAngle", &RotationAngle, 0.1f, 0.f, 6.28318530718f);
            ImGui::DragFloat3("RotationAxisVector", &RotationAxis.x, 0.05, 0.f, 1.f);
            ImGui::DragFloat3("Scaling", &Scaling.x, 0.25f, 0.f, 999.f);
            ImGui::Checkbox("use motionblur", &blur);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}
