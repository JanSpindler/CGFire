//
// Created by JS on 16/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Spline3D.hpp"
#include "engine/Util.hpp"
#include <glm/gtx/compatibility.hpp>

namespace en
{
    Spline3DIterator::Spline3DIterator(uint32_t lP, float lI) :
        lastPoint(lP),
        lastInterp(lI)
    {
    }

    Spline3D::Spline3D(const std::vector<glm::vec3>& controlPoints, bool loop, uint32_t resolution, uint8_t type) :
        controlPoints_(controlPoints),
        loop_(loop),
        resolution_(resolution),
        type_(type)
    {
        controlPoints_ = controlPoints;
        loop_ = loop;

        switch (type)
        {
            case TYPE_CATMULL_ROM:
                ConstructCatmullRom(resolution);
                break;
            case TYPE_NATURAL_CUBIC:
                ConstructNaturalCubic(resolution);
                break;
        }

        GenVao();
    }

    void Spline3D::Recreate(const std::vector<glm::vec3>& controlPoints)
    {
        glDeleteVertexArrays(1, &lineVao_);
        glDeleteVertexArrays(1, &pointVao_);
        controlPoints_ = controlPoints;

        switch (type_)
        {
            case TYPE_CATMULL_ROM:
                ConstructCatmullRom(resolution_);
                break;
            case TYPE_NATURAL_CUBIC:
                ConstructNaturalCubic(resolution_);
                break;
        }

        GenVao();
    }

    void Spline3D::Render(const GLProgram *program) // Deprecated
    {
        // TODO:
    }

    void Spline3D::RenderPosOnly(const GLProgram *program)
    {
        RenderObj::RenderPosOnly(program);
        // This call is used for shadow mapping
        // Point shadow mapping used Triangled in the Geometry Shader
        // Therefore method must not draw any non Triangle primitives
    }

    void Spline3D::RenderDiffuse(const GLProgram *program)
    {
        RenderObj::RenderDiffuse(program);
        program->SetUniformVec4f("diffuse_color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        RenderLines();
        program->SetUniformVec4f("diffuse_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        RenderPoints();
    }

    void Spline3D::RenderAll(const GLProgram *program)
    {
        RenderObj::RenderAll(program);
        RenderDiffuse(program);
    }

    glm::vec3 Spline3D::IterateRelative(Spline3DIterator* iterator, float t) const
    {
        if (t == 0.0f)
            Log::Error("t must be greater than 0.0 to iterate Spline", true);
        if (t < 0.0f)
            Log::Error("Spline3D iteration does not support negative t", true);
        if (iterator->lastPoint < 0 || iterator->lastPoint >= points_.size())
            Log::Error("i->lastPoint exceeded bounds", true);

        t += iterator->lastInterp;

        uint32_t i = iterator->lastPoint;
        while (t > segmentLengths_[i])
        {
            t -= segmentLengths_[i];
            i++;
            if (i >= segmentLengths_.size()){
                i--;
                return glm::vec3(0.f, 0.f, 0.f); // ich will keine loops
                i %= segmentLengths_.size();
            }
        }

        iterator->lastInterp = t;
        iterator->lastPoint = i;

        return glm::lerp(points_[i], points_[i + 1], t / segmentLengths_[i]);
    }

    glm::vec3 Spline3D::IterateAbsolute(float t) const
    {
        if (t < 0.0f)
            Log::Error("t cannot be negative", true);

        while (t > totalLength_) {
            return glm::vec3(0.f, 0.f, 0.f);
            //t -= totalLength_;
        }

        uint32_t i = 0;
        while (segmentLengths_.size()>i && t > segmentLengths_[i])
        {
            t -= segmentLengths_[i];
            i++;
        }
        if (i == points_.size() - 1) {
            return glm::vec3(0.f, 0.f, 0.f);
            i = 0;
        }

        t /= segmentLengths_[i];
        return glm::lerp(points_[i], points_[i + 1], t);
    }

    glm::vec3 Spline3D::GetSegmentDir(uint32_t segmentIndex) const
    {
        segmentIndex %= segmentLengths_.size();
        return glm::normalize(points_[segmentIndex + 1] - points_[segmentIndex]);
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

    float Spline3D::GetTotalLength() const
    {
        return totalLength_;
    }

    uint32_t Spline3D::GetResolution() const{
        return resolution_;
    }
    uint8_t Spline3D::GetType() const{
        return type_;
    }


    void Spline3D::ConstructCatmullRom(uint32_t resolution)
    {
        if (controlPoints_.size() < 4)
            Log::Error("CatmullRom does need at least 4 control points", true);

        const unsigned int controlPointCount = controlPoints_.size();
        const unsigned int pointCount = loop_ ? (resolution * controlPointCount) + 1 : (resolution * (controlPointCount - 1)) + 1;
        const float dt = 1.0f / (float) resolution;
        points_.resize(pointCount);

        // First pair of control points
        glm::vec3 p1 = controlPoints_[0];
        glm::vec3 p2 = controlPoints_[1];
        glm::vec3 p3 = controlPoints_[2];
        glm::vec3 p0 = loop_ ? controlPoints_[controlPointCount - 1] : p1 - glm::normalize(p2 - p1);
        for (unsigned int i = 0; i < resolution; i++)
            points_[i] = CatmullRom(p0, p1, p2, p3, (float) i * dt);

        // Middle pairs of control points
        unsigned int offset;
        for (unsigned int j = 1; j < controlPointCount - 2; j++)
        {
            offset = j * resolution;
            p0 = controlPoints_[j - 1];
            p1 = controlPoints_[j];
            p2 = controlPoints_[j + 1];
            p3 = controlPoints_[j + 2];
            for (unsigned int i = 0; i < resolution; i++)
                points_[offset + i] = CatmullRom(p0, p1, p2, p3, (float) i * dt);
        }

        // Second last pair of control points
        offset = (controlPointCount - 2) * resolution;
        p0 = controlPoints_[controlPointCount - 3];
        p1 = controlPoints_[controlPointCount - 2];
        p2 = controlPoints_[controlPointCount - 1];
        p3 = loop_ ? controlPoints_[0] : p2 + glm::normalize(p2 - p1);
        for (unsigned int i = 0; i < resolution; i++)
            points_[offset + i] = CatmullRom(p0, p1, p2, p3 , (float) i * dt);

        if (loop_)
        {
            // Last pair of control points
            offset = (controlPointCount - 1) * resolution;
            p0 = controlPoints_[controlPointCount - 2];
            p1 = controlPoints_[controlPointCount - 1];
            p2 = controlPoints_[0];
            p3 = controlPoints_[1];
            for (unsigned int i = 0; i < resolution; i++)
                points_[offset + i] = CatmullRom(p0, p1, p2, p3, (float) i * dt);
        }

        // Include first point twice for complete last segment of last pair
        points_[offset + resolution] = CatmullRom(p0, p1, p2, p3, 1.0f);

        // Calculate length of ext_spline
        totalLength_ = 0.0f;
        unsigned int segmentCount = pointCount - 1;
        segmentLengths_.resize(segmentCount);
        for (unsigned int i = 0; i < segmentCount; i++)
        {
            float segmentLength = glm::length(points_[i + 1] - points_[i]);
            segmentLengths_[i] = segmentLength;
            totalLength_ += segmentLength;
        }

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

    void Spline3D::ConstructNaturalCubic(uint32_t resolution)
    {
        if (loop_)
            Log::Error("Natural cubic spline can not create loops yet", true);

        uint32_t n = controlPoints_.size();

        std::vector<float> x(n);
        for (uint32_t i = 0; i < n; i++)
            x[i] = (float) i;

        std::vector<float> h(n - 1);
        for (uint32_t i = 0; i < n - 1; i++)
            h[i] = x[i + 1] - x[i];

        std::vector<float> y0(n);
        std::vector<float> y1(n);
        std::vector<float> y2(n);
        for (uint32_t i = 0; i < n; i++)
        {
            y0[i] = controlPoints_[i][0];
            y1[i] = controlPoints_[i][1];
            y2[i] = controlPoints_[i][2];
        }

        std::vector<float> m0 = CalcNaturalCubicMoments(h, y0, loop_);
        std::vector<float> m1 = CalcNaturalCubicMoments(h, y1, loop_);
        std::vector<float> m2 = CalcNaturalCubicMoments(h, y2, loop_);

        std::vector<glm::vec4> y0Coef = CalcNaturalCubicCoef(h, y0, m0);
        std::vector<glm::vec4> y1Coef = CalcNaturalCubicCoef(h, y1, m1);
        std::vector<glm::vec4> y2Coef = CalcNaturalCubicCoef(h, y2, m2);

        points_.resize((resolution * (n - 1)) + 1);
        float dx = 1.0f / (float) resolution;
        glm::vec3 p;
        for (uint32_t i = 0; i < n - 1; i++)
        {
            for (uint32_t j = 0; j < resolution; j++)
            {
                float realX = dx * (float) j;
                p[0] = NaturalCubic(y0Coef[i], realX);
                p[1] = NaturalCubic(y1Coef[i], realX);
                p[2] = NaturalCubic(y2Coef[i], realX);
                points_[(i * resolution) + j] = p;
            }
        }
        points_[resolution * (n - 1)] = controlPoints_[n - 1];
    }

    // Implementation based on: https://de.wikipedia.org/wiki/Spline-Interpolation#Der_kubische_C%C2%B2-Spline
    // Return: Spline moments
    std::vector<float> Spline3D::CalcNaturalCubicMoments(const std::vector<float> &h, const std::vector<float> &y, bool loop)
    {
        // n + 1 points are given
        uint32_t n = h.size();

        // Calculate l[0] to l[n]
        std::vector<float> d(n + 1);
        d[0] = 0.0f;
        for (uint32_t i = 1; i < n; i++)
        {
            float part0 = y[i + 1] - y[i];
            float part1 = y[i] - y[i - 1];
            d[i] = (part0 / h[i]) - (part1 / h[i - 1]);
        }
        d[n] = 0.0f;

        // Create tridiagonal matrix using 3 vectors
        std::vector<float> lowDiag(n + 1);
        std::vector<float> midDiag(n + 1);
        std::vector<float> highDiag(n + 1);
        lowDiag[0] = 0.0f;
        midDiag[0] = 1.0f;
        highDiag[0] = 0.0f;
        for (uint32_t i = 1; i < n; i++)
        {
            lowDiag[i] = h[i - 1] / 6.0f;
            midDiag[i] = (h[i - 1] + h[i]) / 3.0f;
            highDiag[i] = h[i] / 6.0f;
        }
        lowDiag[n] = 0.0f;
        midDiag[n] = 1.0f;
        highDiag[n] = 0.0f;

        // Solve linear equation using Thomas Algorithm (tridiagonal matrices)
        const std::vector<float>& a = lowDiag;
        const std::vector<float>& b = midDiag;
        const std::vector<float>& c = highDiag;

        std::vector<float> c_(n + 1);
        c_[0] = c[0] / b[0];
        for (uint32_t i = 1; i < n + 1; i++)
            c_[i] = c[i] / (b[i] - (c_[i - 1] * a[i]));

        std::vector<float> d_(n + 1);
        d_[0] = d[0] / b[0];
        for (uint32_t i = 1; i < n + 1; i++)
        {
            float part0 = d[i] - (d_[i - 1] * a[i]);
            float part1 = b[i] - (c_[i - 1] * a[i]);
            d_[i] = part0 / part1;
        }

        std::vector<float> m(n + 1);
        m[n] = d_[n];
        for (uint32_t i = n - 1; i > 0; i--)
            m[i] = d_[i] - (c_[i] * m[i + 1]);

        return m;
    }

    std::vector<glm::vec4> Spline3D::CalcNaturalCubicCoef(
            const std::vector<float>& h,
            const std::vector<float>& y,
            const std::vector<float>& m)
    {
        uint32_t n = h.size();
        std::vector<glm::vec4> coef(n);
        for (uint32_t i = 0; i < n; i++)
        {
            coef[i][0] = y[i];
            coef[i][1] = ((y[i + 1] - y[i]) / h[i]) - ((h[i] / 6.0f) * (m[i + 1] + (2 * m[i])));
            coef[i][2] = m[i] / 2.0f;
            coef[i][3] = (m[i + 1] - m[i]) / (6 * h[i]);
        }
        return coef;
    }

    float Spline3D::NaturalCubic(glm::vec4 coef, float x)
    {
        float x2 = x * x;
        float x3 = x2 * x;

        float part0 = coef[0];
        float part1 = coef[1] * x;
        float part2 = coef[2] * x2;
        float part3 = coef[3] * x3;

        return part0 + part1 + part2 + part3;
    }

    void Spline3D::GenVao()
    {
        // Line
        glGenVertexArrays(1, &lineVao_);
        glBindVertexArray(lineVao_);

        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, points_.size() * sizeof(glm::vec3), points_.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

        glBindVertexArray(0);

        // Control points
        glGenVertexArrays(1, &pointVao_);
        glBindVertexArray(pointVao_);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, controlPoints_.size() * sizeof(glm::vec3), controlPoints_.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

        glBindVertexArray(0);
    }

    void Spline3D::RenderLines() const
    {
        glBindVertexArray(lineVao_);
        glDrawArrays(GL_LINE_STRIP, 0, points_.size());
        glBindVertexArray(0);
    }

    void Spline3D::RenderPoints() const
    {
        glPointSize(8.0f);
        glBindVertexArray(pointVao_);
        glDrawArrays(GL_POINTS, 0, controlPoints_.size());
        glBindVertexArray(0);
    }


    bool Spline3D::IsRenderObjTypePossible(en::RenderObjType type) const{
        switch(type){
            case RenderObjType::Spline:
                return true;
            default:
                return false;
        }
    }

    void Spline3D::OnImGuiRender(){
        if (ImGui::Checkbox("loop", &loop_)){
            this->Recreate(controlPoints_);
        }

        if (ImGui::DragInt("Resolution", &resolution_, 0.05, controlPoints_.size() * 4, 999)){
            this->Recreate(controlPoints_);
        }

        if (ImGui::Combo("Type ", &type_, "CATMULL_ROM\0NATURAL_CUBIC\0")){
            this->Recreate(controlPoints_);
        }

        int i = 0;
        for (auto it = controlPoints_.begin(); it != controlPoints_.end(); ++it){
            ImGui::PushID(i);
            if (ImGui::DragFloat3("", &(*it).x, 0.05f)){
                this->Recreate(controlPoints_);
            }
            ImGui::SameLine();
            if(ImGui::Button("Clone")){
                controlPoints_.insert(it, (*it));

                this->Recreate(controlPoints_);
                ImGui::PopID();
                break;

            }
            ImGui::SameLine();
            if(ImGui::Button("Delete")){
                if (controlPoints_.size() > 4) {
                    controlPoints_.erase(it);
                    this->Recreate(controlPoints_);
                    ImGui::PopID();
                    break;
                }
            }

            ImGui::PopID();
            i++;
        }
    }
}
