//
// Created by JS on 16/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Spline3D.hpp"
#include "engine/Util.hpp"

namespace en
{
    Spline3D::Spline3D(const std::vector<glm::vec3>& controlPoints, bool loop, unsigned int resolution)
    {
        if (controlPoints.size() < 4)
            Log::Error("At least 4 control points needed for Spline3D", true);

        controlPoints_ = controlPoints;
        loop_ = loop;

        const unsigned int controlPointCount = controlPoints.size();
        const unsigned int pointCount = loop ? resolution * controlPointCount : resolution * (controlPointCount - 1) + 1;
        const float dt = 1.0f / (float) resolution;
        points_.resize(pointCount);

        // First pair of control points
        glm::vec3 p1 = controlPoints[0];
        glm::vec3 p2 = controlPoints[1];
        glm::vec3 p3 = controlPoints[2];
        glm::vec3 p0 = loop ? controlPoints[controlPointCount - 1] : p1 - glm::normalize(p2 - p1);
        for (unsigned int i = 0; i < resolution; i++)
            points_[i] = CatmullRom(p0, p1, p2, p3, (float) i * dt);

        // Middle pairs of control points
        unsigned int offset;
        for (unsigned int j = 1; j < controlPointCount - 2; j++)
        {
            offset = j * resolution;
            p0 = controlPoints[j - 1];
            p1 = controlPoints[j];
            p2 = controlPoints[j + 1];
            p3 = controlPoints[j + 2];
            for (unsigned int i = 0; i < resolution; i++)
                points_[offset + i] = CatmullRom(p0, p1, p2, p3, (float) i * dt);
        }

        // Second last pair of control points
        offset = (controlPointCount - 2) * resolution;
        p0 = controlPoints[controlPointCount - 3];
        p1 = controlPoints[controlPointCount - 2];
        p2 = controlPoints[controlPointCount - 1];
        p3 = loop ? controlPoints[0] : p2 + glm::normalize(p2 - p1);
        for (unsigned int i = 0; i < resolution; i++)
            points_[offset + i] = CatmullRom(p0, p1, p2, p3 , (float) i * dt);

        if (loop)
        {
            // Last pair of control points
            offset = (controlPointCount - 1) * resolution;
            p0 = controlPoints[controlPointCount - 2];
            p1 = controlPoints[controlPointCount - 1];
            p2 = controlPoints[0];
            p3 = controlPoints[1];
            for (unsigned int i = 0; i < resolution; i++)
                points_[offset + i] = CatmullRom(p0, p1, p2, p3, (float) i * dt);
        }
        else
        {
            // Complete last subpair of second last control point pair
            points_[offset + resolution] = CatmullRom(p0, p1, p2, p3, 1.0f);
        }

        // Calculate length of spline
        length_ = 0.0f;
        for (unsigned int i = 0; i < pointCount - 1; i++)
            length_ += glm::length(points_[i + 1] - points_[i]);
    }

    glm::vec3 Spline3D::GetPoint(float t) const
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

    float Spline3D::GetLength() const
    {
        return length_;
    }

    glm::vec3 Spline3D::CatmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
    {
        float t2 = t * t;
        float t3 = t2 * t;
        glm::vec3 part0 = 2.0f * p1;
        glm::vec3 part1 = (-p0 + p2) * t;
        glm::vec3 part2 = (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2;
        glm::vec3 part3 = (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3;
        return 0.5f * (part0 + part1 + part2 + part3);
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
