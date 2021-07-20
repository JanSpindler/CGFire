//
// Created by JS on 27/05/2021.
//

#include "engine/render/Renderable.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <framework/imgui_util.hpp>
#include "engine/Spline3D.hpp"
#include "engine/Util.hpp"

namespace en
{
    RenderObj::RenderObj()
    :   spline_(nullptr),
        iterator_(nullptr)
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

    void RenderObj::TrackSpline(const Spline3D* spline, float trackSpeed)
    {
        assert(spline != nullptr);
        spline_ = spline;
        trackSpeed_ = trackSpeed;
        iterator_ = new Spline3DIterator(0, 0.0f);
    }

    void RenderObj::TrackStep(float deltaTime)
    {
        if (spline_ != nullptr){
            assert(iterator_ != nullptr);
            Position = spline_->IterateRelative(iterator_, trackSpeed_ * deltaTime);
            Quaternion = glm::quatLookAt(-spline_->GetSegmentDir(iterator_->lastPoint), glm::vec3(0.f, 1.f, 0.f));
        }
    }
    void RenderObj::StopSplineTracking(){
        if (spline_){
            spline_ = nullptr;
            delete iterator_;
            iterator_ = nullptr;
        }
    }

    glm::vec3 RenderObj::GetPos()
    {
        return glm::vec3(t_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void RenderObj::SetMatrices(const GLProgram *program)
    {
        auto trans = glm::translate(glm::mat4(1.f), Position);
        auto rot = glm::toMat4(Quaternion);
        auto scale = glm::scale(glm::mat4(1.f), Scaling);
        t_ =  trans * rot * scale;

        program->SetUniformMat4("model_mat", false, &t_[0][0]);
        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(t_)));
        program->SetUniformMat3("normal_mat", false, &normalMat[0][0]);
        program->SetUniformMat4("prevM", false, &prevM[0][0]);
        prevM = t_;
    }
}
