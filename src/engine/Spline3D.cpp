//
// Created by JS on 16/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Spline3D.hpp"
#include "engine/Util.hpp"

namespace en
{
    Spline3D::Spline3D(const std::vector<glm::vec3>& controlPoints, bool loop)
    {
        controlPoints_ = controlPoints;
        points_ = controlPoints;
        loop_ = loop;
    }

    glm::vec3 Spline3D::GetPoint(float x) const
    {
        // TODO: implement correctly
        return glm::vec3(0.0f);
    }

    unsigned int Spline3D::GetControlPointCount() const
    {
        return controlPoints_.size();
    }

    const std::vector<glm::vec3>& Spline3D::GetControlPoints() const
    {
        return controlPoints_;
    }

    unsigned int Spline3D::GetPointCount() const
    {
        return points_.size();
    }

    const std::vector<glm::vec3>& Spline3D::GetPoints() const
    {
        return points_;
    }

    bool Spline3D::IsLooped() const
    {
        return loop_;
    }

    Spline3DRenderable::Spline3DRenderable(const Spline3D *spline)
    {
        spline_ = spline;

        // Line
        glGenVertexArrays(1, &lineVao_);
        glBindVertexArray(lineVao_);

        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        const std::vector<glm::vec3>& points_ = spline_->GetPoints();
        glBufferData(GL_ARRAY_BUFFER, points_.size() * sizeof(glm::vec3), points_.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

        glBindVertexArray(0);

        // Control points
        glGenVertexArrays(1, &pointVao_);
        glBindVertexArray(pointVao_);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        const std::vector<glm::vec3>& controlPoints = spline_->GetControlPoints();
        glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), controlPoints.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

        glBindVertexArray(0);
    }

    void Spline3DRenderable::Render(const GLProgram *program) const
    {
        // Draw line
        program->SetUniformVec4f("fixed_color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        glBindVertexArray(lineVao_);
        GLenum mode = spline_->IsLooped() ? GL_LINE_LOOP : GL_LINE_STRIP;
        glDrawArrays(mode, 0, spline_->GetPointCount());
        glBindVertexArray(0);

        // Draw controlPoints
        program->SetUniformVec4f("fixed_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        glPointSize(8.0f);
        glBindVertexArray(pointVao_);
        glDrawArrays(GL_POINTS, 0, spline_->GetControlPointCount());
        glBindVertexArray(0);
    }
}
